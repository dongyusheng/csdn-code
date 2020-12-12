package session

import (
	"errors"
	"sync"
	"time"

	"github.com/garyburd/redigo/redis"
	uuid "github.com/satori/go.uuid"
)

// RedisSeesionMgr 管理缓存中的所有RedisSession对象
type RedisSeesionMgr struct {
	addr       string
	password   string
	pool       *redis.Pool
	sessionMap map[string]Session // 每个用户对应一项
	rwlock     sync.RWMutex
}

// RedisSeesionMgr 构造函数
func NewRedisSeesionMgr() SessionMgr {
	sr := &RedisSeesionMgr{
		sessionMap: make(map[string]Session, 1024),
	}
	return sr
}

func (r *RedisSeesionMgr) Init(addr string, options ...string) (err error) {
	if len(options) > 0 {
		r.password = options[0]
	}
	r.addr = addr

	// 创建Redis连接池
	myPool(r.addr, r.password)

	return
}

func myPool(addr, password string) *redis.Pool {
	return &redis.Pool{
		MaxIdle:     64,
		MaxActive:   1000,
		IdleTimeout: 240 * time.Second,
		Dial: func() (redis.Conn, error) {
			conn, err := redis.Dial("tcp", addr)
			if err != nil {
				return nil, err
			}

			// 若有密码, 判断
			if _, err := conn.Do("AUTH", password); err != nil {
				conn.Close()
				return nil, err
			}
			return conn, err
		},
		// 连接测试, 开发时写, 上线时去除
		TestOnBorrow: func(conn redis.Conn, t time.Time) error {
			_, err := conn.Do("PING")
			return err
		},
	}
}

func (r *RedisSeesionMgr) CreateSession() (session Session, err error) {
	r.rwlock.Lock()
	defer r.rwlock.Unlock()

	// 用uuid作为sessionId
	id := uuid.NewV4()
	if err != nil {
		return
	}

	// 创建个session, 加入map中
	sessionId := id.String()
	session = NewRedisSession(sessionId, r.pool)
	r.sessionMap[sessionId] = session

	return
}

func (r *RedisSeesionMgr) Get(sessionId string) (session Session, err error) {
	r.rwlock.Lock()
	defer r.rwlock.Unlock()

	// 直接从map中取值返回
	session, ok := r.sessionMap[sessionId]
	if !ok {
		err = errors.New("session not exists")
		return
	}

	return
}
