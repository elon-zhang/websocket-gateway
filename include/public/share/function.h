#ifndef SHARED_FUNCTION_H
#define SHARED_FUNCTION_H

#include <exception>
#include <cstddef>

namespace shared
{
    class null_function_exception : public std::exception
    {
    public:
        virtual const char* what() const throw() {
            return "null function object called";
        }
    };

    namespace detail
    {
        template<typename T>
        struct __function_impl;

        template<typename R>
        struct __function_impl<R()> {
            virtual ~__function_impl() {}
            virtual R operator()() = 0;
        };

        template<typename R, typename A1>
        struct __function_impl<R(A1)> {
            virtual ~__function_impl() {}
            virtual R operator()(A1 a1) = 0;
        };

        template<typename R, typename A1, typename A2>
        struct __function_impl<R(A1, A2)> {
            virtual ~__function_impl() {}
            virtual R operator()(A1 a1, A2 a2) = 0;
        };

        template<typename R, typename A1, typename A2, typename A3>
        struct __function_impl<R(A1, A2, A3)> {
            virtual ~__function_impl() {}
            virtual R operator()(A1 a1, A2 a2, A3 a3) = 0;
        };
    }

    template<typename T>
    class function;

    /// arguments 0
    template<typename R>
    class function<R()>
    {
    public:
        typedef detail::__function_impl<R()> impl_t;
        function() : impl_(NULL) {}
        function(impl_t* impl) : impl_(impl) {}
        function(const function<R()>& rhs) : impl_(rhs.impl_) {
            rhs.impl_ = NULL;
        }
        const function<R()>& operator=(const function<R()>& rhs) {
            if (impl_) {
                delete impl_;
            }
            impl_ = rhs.impl_;
            rhs.impl_ = NULL;
            return *this;
        }
        ~function() {
            if (impl_) {
                delete impl_;
                impl_ = NULL;
            }
        }
        operator bool() const {
            return impl_ != NULL;
        }

    public:
        R operator()() {
            if (impl_ == NULL) {
                throw null_function_exception();
            }
            return (*impl_)();
        }

    private:
        mutable impl_t* impl_;
    };

    /// arguments 1
    template<typename R, typename A1>
    class function<R(A1)>
    {
    public:
        typedef detail::__function_impl<R(A1)> impl_t;
        function() : impl_(NULL) {}
        function(impl_t* impl) : impl_(impl) {}
        function(const function<R(A1)>& rhs) : impl_(rhs.impl_) {
            rhs.impl_ = NULL;
        }
        const function<R(A1)>& operator=(const function<R(A1)>& rhs) const {
            if (impl_) {
                delete impl_;
            }
            impl_ = rhs.impl_;
            rhs.impl_ = NULL;
            return *this;
        }
        ~function() {
            if (impl_) {
                delete impl_;
                impl_ = NULL;
            }
        }
        operator bool() const {
            return impl_ != NULL;
        }

    public:
        R operator()(A1 a1) {
            if (impl_ == NULL) {
                throw null_function_exception();
            }
            return (*impl_)(a1);
        }

    private:
        mutable impl_t* impl_;
    };

    /// arguments 2
    template<typename R, typename A1, typename A2>
    class function<R(A1, A2)>
    {
    public:
        typedef detail::__function_impl<R(A1, A2)> impl_t;
        function() : impl_(NULL) {}
        function(impl_t* impl) : impl_(impl) {}
        function(const function<R(A1, A2)>& rhs) : impl_(rhs.impl_) {
            rhs.impl_ = NULL;
        }
        const function<R(A1, A2)>& operator=(const function<R(A1, A2)>& rhs) const {
            if (impl_) {
                delete impl_;
            }
            impl_ = rhs.impl_;
            rhs.impl_ = NULL;
            return *this;
        }
        ~function() {
            if (impl_) {
                delete impl_;
                impl_ = NULL;
            }
        }
        operator bool() const {
            return impl_ != NULL;
        }

    public:
        R operator()(A1 a1, A2 a2) {
            if (impl_ == NULL) {
                throw null_function_exception();
            }
            return (*impl_)(a1, a2);
        }

    private:
        mutable impl_t* impl_;
    };

    /// arguments 3
    template<typename R, typename A1, typename A2, typename A3>
    class function<R(A1, A2, A3)>
    {
    public:
        typedef detail::__function_impl<R(A1, A2, A3)> impl_t;
        function() : impl_(NULL) {}
        function(impl_t* impl) : impl_(impl) {}
        function(const function<R(A1, A2, A3)>& rhs) : impl_(rhs.impl_) {
            rhs.impl_ = NULL;
        }
        const function<R(A1, A2, A3)>& operator=(const function<R(A1, A2, A3)>& rhs) const {
            if (impl_) {
                delete impl_;
            }
            impl_ = rhs.impl_;
            rhs.impl_ = NULL;
            return *this;
        }
        ~function() {
            if (impl_) {
                delete impl_;
                impl_ = NULL;
            }
        }
        operator bool() const {
            return impl_ != NULL;
        }

    public:
        R operator()(A1 a1, A2 a2, A3 a3) {
            if (impl_ == NULL) {
                throw null_function_exception();
            }
            return (*impl_)(a1, a2, a3);
        }

    private:
        mutable impl_t* impl_;
    };

    namespace detail
    {
        template<typename T, typename R>
        class __member_function_impl0 : public __function_impl<R()>
        {
        public:
            typedef R(T::*memfun_t)();
            __member_function_impl0(T* obj, memfun_t f)
                : obj_(obj), fun_(f) {}

            virtual R operator()() {
                return (obj_->*fun_)();
            }

        private:
            T* obj_;
            memfun_t fun_;
        };

        template<typename R>
        class __static_function_impl0 : public __function_impl<R()>
        {
        public:
            typedef R(*fun_t)();
            __static_function_impl0(fun_t f)
                : fun_(f) {}

            virtual R operator()() {
                return (*fun_)();
            }

        private:
            fun_t fun_;
        };

        template<typename T, typename R, typename A1>
        class __member_function_impl1 : public __function_impl<R(A1)>
        {
        public:
            typedef R(T::*memfun_t)(A1);
            __member_function_impl1(T* obj, memfun_t f)
                : obj_(obj), fun_(f) {}

            virtual R operator()(A1 a1) {
                return (obj_->*fun_)(a1);
            }
        private:
            T* obj_;
            memfun_t fun_;
        };

        template<typename R, typename A1>
        class __static_function_impl1 : public __function_impl<R(A1)>
        {
        public:
            typedef R(*fun_t)(A1);
            __static_function_impl1(fun_t f)
                : fun_(f) {}

            virtual R operator()(A1 a1) {
                return (*fun_)(a1);
            }

        private:
            fun_t fun_;
        };

        template<typename T, typename R, typename A1, typename A2>
        class __member_function_impl2 : public __function_impl<R(A1, A2)>
        {
        public:
            typedef R(T::*memfun_t)(A1, A2);
            __member_function_impl2(T* obj, memfun_t f)
                : obj_(obj), fun_(f) {}

            virtual R operator()(A1 a1, A2 a2) {
                return (obj_->*fun_)(a1, a2);
            }
        private:
            T* obj_;
            memfun_t fun_;
        };

        template<typename R, typename A1, typename A2>
        class __static_function_impl2 : public __function_impl<R(A1, A2)>
        {
        public:
            typedef R(*fun_t)(A1, A2);
            __static_function_impl2(fun_t f)
                : fun_(f) {}

            virtual R operator()(A1 a1, A2 a2) {
                return (*fun_)(a1, a2);
            }

        private:
            fun_t fun_;
        };

        template<typename T, typename R, typename A1, typename A2, typename A3>
        class __member_function_impl3 : public __function_impl<R(A1, A2, A3)>
        {
        public:
            typedef R(T::*memfun_t)(A1, A2, A3);
            __member_function_impl3(T* obj, memfun_t f)
                : obj_(obj), fun_(f) {}

            virtual R operator()(A1 a1, A2 a2, A3 a3) {
                return (obj_->*fun_)(a1, a2, a3);
            }
        private:
            T* obj_;
            memfun_t fun_;
        };

        template<typename R, typename A1, typename A2, typename A3>
        class __static_function_impl3 : public __function_impl<R(A1, A2, A3)>
        {
        public:
            typedef R(*fun_t)(A1, A2, A3);
            __static_function_impl3(fun_t f)
                : fun_(f) {}

            virtual R operator()(A1 a1, A2 a2, A3 a3) {
                return (*fun_)(a1, a2, a3);
            }

        private:
            fun_t fun_;
        };
    }

    template<typename T, typename R>
    function<R()> bind(R(T::*fun)(), T* obj)
    {
        return function<R()>(new detail::__member_function_impl0<T, R>(obj, fun));
    }

    template<typename R>
    function<R()> bind(R(*fun)())
    {
        return function<R()>(new detail::__static_function_impl0<R>(fun));
    }



    template<typename T, typename R, typename A1>
    function<R(A1)> bind(R(T::*fun)(A1), T* obj)
    {
        return function<R(A1)>(new detail::__member_function_impl1<T, R, A1>(obj, fun));
    }

    template<typename R, typename A1>
    function<R(A1)> bind(R(*fun)(A1))
    {
        return function<R(A1)>(new detail::__static_function_impl1<R, A1>(fun));
    }

    template<typename T, typename R, typename A1, typename A2>
    function<R(A1, A2)> bind(R(T::*fun)(A1, A2), T* obj)
    {
        return function<R(A1, A2)>(new detail::__member_function_impl2<T, R, A1, A2>(obj, fun));
    }

    template<typename R, typename A1, typename A2>
    function<R(A1, A2)> bind(R(*fun)(A1, A2))
    {
        return function<R(A1, A2)>(new detail::__static_function_impl2<R, A1, A2>(fun));
    }

    template<typename T, typename R, typename A1, typename A2, typename A3>
    function<R(A1, A2, A3)> bind(R(T::*fun)(A1, A2, A3), T* obj)
    {
        return function<R(A1, A2, A3)>(new detail::__member_function_impl3<T, R, A1, A2, A3>(obj, fun));
    }

    template<typename R, typename A1, typename A2, typename A3>
    function<R(A1, A2, A3)> bind(R(*fun)(A1, A2, A3))
    {
        return function<R(A1, A2, A3)>(new detail::__static_function_impl3<R, A1, A2, A3>(fun));
    }
}

#endif
