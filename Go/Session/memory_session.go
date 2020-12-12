package session

import (
	"errors"
	"sync"
)

// 用户内存(非缓存)Session

// 每一个用户都对应一个MemorySession对象,
// 然后所有的MemorySession对象会被MemorySeesionMgr管理
type MemorySession struct {
	sessionId string                 // 用户sessionid
	data      map[string]interface{} // 存kv
	rwlock    sync.RWMutex           // 读写锁
}

// 构造函数
func NewMemorySession(id string) *MemorySession {
	s := &MemorySession{
		sessionId: id,
		data:      make(map[string]interface{}, 16),
	}
	return s
}

func (m *MemorySession) Set(key string, value interface{}) (err error) {
	// 加锁
	m.rwlock.Lock()
	defer m.rwlock.Unlock()

	// 设置值
	m.data[key] = value
	return
}

func (m *MemorySession) Get(key string) (value interface{}, err error) {
	// 加锁
	m.rwlock.Lock()
	defer m.rwlock.Unlock()

	// 获取值
	value, ok := m.data[key]
	if !ok {
		err = errors.New("key not exists in session")
		return
	}
	return
}

func (m *MemorySession) Del(key string) (err error) {
	// 加锁
	m.rwlock.Lock()
	defer m.rwlock.Unlock()

	// 删除session
	delete(m.data, key)
	return
}

func (m *MemorySession) Save() (err error) {
	return
}
