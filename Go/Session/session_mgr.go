package session

// MemorySeesionMgr和RedisSeesionMgr会实现这个接口
type SessionMgr interface {
	// 初始化
	Init(addr string, options ...string) (err error)
	CreateSession() (session Session, err error)
	Get(sessionId string) (session Session, err error)
}
