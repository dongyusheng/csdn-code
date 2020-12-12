package session

import (
	"encoding/json"
	"errors"
	"sync"

	"github.com/garyburd/redigo/redis"
)

// 用户缓存Session

// 每一个用户都对应一个RedisSession对象,
// 然后所有的RedisSession对象会被RedisSessionMgr管理
type RedisSession struct {
	sessionId  string      // 用户sessionid
	pool       *redis.Pool // 连接池
	sessionMap map[string]interface{}
	rwlock     sync.RWMutex // 读写锁
	flag       int          // 记录内存中的mao是否被操作
}

const (
	SessionFlagNone   = iota // 内存数据没变化
	SessionFlagModify        // 内存数据有变化
)

// 构造函数
func NewRedisSession(id string, pool *redis.Pool) *RedisSession {
	s := &RedisSession{
		sessionId:  id,
		pool:       pool,
		sessionMap: make(map[string]interface{}, 16),
		flag:       SessionFlagNone,
	}
	return s
}

func (r *RedisSession) Set(key string, value interface{}) (err error) {
	r.rwlock.Lock()
	defer r.rwlock.Unlock()

	// 设置值, 并更改状态
	r.sessionMap[key] = value
	r.flag = SessionFlagModify
	return
}

func (r *RedisSession) Get(key string) (result interface{}, err error) {
	r.rwlock.Lock()
	defer r.rwlock.Unlock()

	// 取值返回
	result, ok := r.sessionMap[key]
	if !ok {
		err = errors.New("key not exists")
	}

	return
}

func (r *RedisSession) loadFromRedis() (err error) {
	conn := r.pool.Get()
	reply, err := conn.Do("GET", r.sessionId)
	if err != nil {
		return
	}

	// 把结果转换为字符串
	data, err := redis.String(reply, err)
	if err != nil {
		return
	}

	// 因为redis中的数据都是序列化的内容, 此处反序列化
	err = json.Unmarshal([]byte(data), &r.sessionMap)
	if err != nil {
		return
	}

	return
}

func (r *RedisSession) Del(key string) (err error) {
	r.rwlock.Lock()
	defer r.rwlock.Unlock()

	// 更改状态, 然后删除
	r.flag = SessionFlagModify
	delete(r.sessionMap, key)

	return
}

func (r *RedisSession) Save() (err error) {
	r.rwlock.Lock()
	defer r.rwlock.Unlock()

	// 如果数据没有变化, 不需要存
	if r.flag != SessionFlagModify {
		return
	}

	// 把数据序列化
	data, err := json.Marshal(r.sessionMap)
	if err != nil {
		return
	}

	// 获取redis连接
	conn := r.pool.Get()
	// 保存到redis中(数据是序列化后的结果)
	_, err = conn.Do("SET", r.sessionId, string(data))
	r.flag = SessionFlagNone // 更改状态
	if err != nil {
		return
	}

	return
}
