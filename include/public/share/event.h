#ifndef __SHARED_EVENT_H__
#define __SHARED_EVENT_H__

//#include "function_observer.h"
#include <vector>

namespace shared
{
    template<typename T>
    class Event;

    /// arguments 0
    template<>
    class Event<void()>
    {
    private:
        struct handler_t {
            handler_t(const function<void()>& h, bool c) : handler(h), constant(c), removed(false) {}
            function<void()> handler;
            bool constant;
            bool removed;
        };

    public:
        void attach(const function<void()>& hd) {
            handlers_.push_back(handler_t(hd, true));
        }
        void once(const function<void()>& hd) {
            handlers_.push_back(handler_t(hd, false));
        }
        void trigger() {
            for (std::size_t i = 0; i < handlers_.size(); ++i) {
                handler_t& hd = handlers_[i];
                if (!hd.removed) {
                    hd.handler();
                    if (!hd.constant) {
                        hd.removed = true;
                    }
                }
            }
            for (std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end();) {
                if ((*it).removed) {
                    it = handlers_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        void clear() {
            for (std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end(); ++it) {
                (*it).removed = true;
            }
        }

    private:
        std::vector<handler_t> handlers_;
    };

    /// arguments 1
    template<typename A1>
    class Event<void(A1)>
    {
    private:
        struct handler_t {
            handler_t(const function<void(A1)>& h, bool c) : handler(h), constant(c), removed(false) {}
            function<void(A1)> handler;
            bool constant;
            bool removed;
        };

    public:
        void attach(const function<void(A1)>& hd) {
            handlers_.push_back(handler_t(hd, true));
        }
        void once(const function<void(A1)>& hd) {
            handlers_.push_back(handler_t(hd, false));
        }
        void trigger(A1 a1) {
            for (std::size_t i = 0; i < handlers_.size(); ++i) {
                handler_t& hd = handlers_[i];
                if (!hd.removed) {
                    hd.handler(a1);
                    if (!hd.constant) {
                        hd.removed = true;
                    }
                }
            }
            for (typename std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end();) {
                if ((*it).removed) {
                    it = handlers_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        void clear() {
            for (typename std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end(); ++it) {
                (*it).removed = true;
            }
        }

    private:
        std::vector<handler_t> handlers_;
    };

    /// arguments 2
    template<typename A1, typename A2>
    class Event<void(A1, A2)>
    {
    private:
        struct handler_t {
            handler_t(const function<void(A1, A2)>& h, bool c) : handler(h), constant(c), removed(false) {}
            function<void(A1, A2)> handler;
            bool constant;
            bool removed;
        };

    public:
        void attach(const function<void(A1, A2)>& hd) {
            handlers_.push_back(handler_t(hd, true));
        }
        void once(const function<void(A1, A2)>& hd) {
            handlers_.push_back(handler_t(hd, false));
        }
        void trigger(A1 a1, A2 a2) {
            for (std::size_t i = 0; i < handlers_.size(); ++i) {
                handler_t& hd = handlers_[i];
                if (!hd.removed) {
                    hd.handler(a1, a2);
                    if (!hd.constant) {
                        hd.removed = true;
                    }
                }
            }
            for (typename std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end();) {
                if ((*it).removed) {
                    it = handlers_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        void clear() {
            for (typename std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end(); ++it) {
                (*it).removed = true;
            }
        }

    private:
        std::vector<handler_t> handlers_;
    };

    /// arguments 3
    template<typename A1, typename A2, typename A3>
    class Event<void(A1, A2, A3)>
    {
    private:
        struct handler_t {
            handler_t(const function<void(A1, A2, A3)>& h, bool c) : handler(h), constant(c), removed(false) {}
            function<void(A1, A2, A3)> handler;
            bool constant;
            bool removed;
        };

    public:
        void attach(const function<void(A1, A2, A3)>& hd) {
            handlers_.push_back(handler_t(hd, true));
        }
        void once(const function<void(A1, A2, A3)>& hd) {
            handlers_.push_back(handler_t(hd, false));
        }
        void trigger(A1 a1, A2 a2, A3 a3) {
            for (std::size_t i = 0; i < handlers_.size(); ++i) {
                handler_t& hd = handlers_[i];
                if (!hd.removed) {
                    hd.handler(a1, a2, a3);
                    if (!hd.constant) {
                        hd.removed = true;
                    }
                }
            }
            for (typename std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end();) {
                if ((*it).removed) {
                    it = handlers_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        void clear() {
            for (typename std::vector<handler_t>::iterator it = handlers_.begin(); it != handlers_.end(); ++it) {
                (*it).removed = true;
            }
        }

    private:
        std::vector<handler_t> handlers_;
    };
}

#endif
