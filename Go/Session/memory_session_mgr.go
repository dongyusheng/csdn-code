package session

import (
	"errors"
	"sync"

	uuid "github.com/satori/go.uuid"
)

// MemorySeesionMgr管理内存(非缓存)中的所有MemorySession对象
type MemorySeesionMgr struct {
	sessionMap map[string]Session // 每个用户对应一项
	rwlock     sync.RWMutex
}

// MemorySeesionMgr 构造函数
func NewMemorySeesionMgr() SessionMgr {
	sr := &MemorySeesionMgr{
		sessionMap: make(map[string]Session, 1024),
	}
	return sr
}

func (s *MemorySeesionMgr) Init(addr string, options ...string) (err error) {
	return
}

func (s *MemorySeesionMgr) CreateSession() (session Session, err error) {
	s.rwlock.Lock()
	defer s.rwlock.Unlock()

	// 用uuid作为sessionId
	id:= uuid.NewV4()
	if err != nil {
		return
	}

	// 创建个session, 加入map中
	sessionId := id.String()
	session = NewMemorySession(sessionId)
	s.sessionMap[sessionId] = session

	return
}

func (s *MemorySeesionMgr) Get(sessionId string) (session Session, err error) {
	s.rwlock.Lock()
	defer s.rwlock.Unlock()

	// 直接从map中取值返回
	session, ok := s.sessionMap[sessionId]
	if !ok {
		err = errors.New("session not exists")
		return
	}

	return
}
