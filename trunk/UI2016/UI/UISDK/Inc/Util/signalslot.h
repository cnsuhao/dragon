#ifndef SIGNALSLOT_H
#define SIGNALSLOT_H
#include <list>
// 信号/槽 实现

namespace UI
{
	enum slot_type
	{
		slot_function = 0,
		slot_method = 1,
	};

#if _MSC_VER >= 1800 //  (Visual C++ 2013 才开始支持变长模板参数)，

    // 全局函数/成员函数基类，用于在signal类中统一处理
    template<typename Return, typename... Args>
    class slot_base
    {
    public:
		virtual slot_type type() = 0;
        virtual Return invoke(Args... args) = 0;
        virtual void  release() = 0;
    };

    // 全局函数封装
    template<typename Return, typename... Args>
    class function_slot : public slot_base<Return, Args...>
    {
    public:
        typedef Return(*Function)(Args...);
		function_slot(Function f) : m_func(f) {
		}

		bool equal(Function f){
			return f == m_func;
		}
		virtual slot_type type() override { 
			return slot_function; 
		}
        virtual Return invoke(Args... args) override {
			return m_func(args...);
        }
        virtual void  release() override {
            delete this;
        }

    private:
		Function m_func;
    };

	// 无参数回调，即绑定时可以不提供参数
	template<typename Return, typename... Args>
	class function_slot0 : public slot_base<Return, Args...>
	{
	public:
		typedef Return(*Function)();
		function_slot0(Function f) : m_func(f) {
		}

		bool equal(Function f){
			return f == m_func;
		}
		virtual slot_type type() override {
			return slot_function;
		}
		virtual Return invoke(Args... args) override {
			return m_func();
		}
		virtual void  release() override {
			delete this;
		}

	private:
		Function m_func;
	};

    // 成员函数封装
    template<typename Return, typename Class, typename... Args>
    class method_slot : public slot_base<Return, Args...>
    {
    public:
        typedef Return(Class::*Function)(Args...);

        method_slot(Class* pThis, Function f) : 
                m_this(pThis), m_func(f) {
		}

		bool equal(Class* pThis, Function f){
			return pThis == m_this && f == m_func;
		}
		bool equal(Class* pThis){
			return pThis == m_this;
		}
		virtual slot_type type() override { 
			return slot_method; 
		}
        virtual Return invoke(Args... args) override {
            return (m_this->*m_func)(args...);
        }
        virtual void  release() override {
            delete this;
        }

    private:
        Function  m_func;
        Class* m_this;
    };

	// 无参数 成员函数 版本，绑定者可不提供参数
	template<typename Return, typename Class, typename... Args>
	class method_slot0 : public slot_base<Return, Args...>
	{
	public:
		typedef Return(Class::*Function)();

		method_slot0(Class* pThis, Function f) :
			m_this(pThis), m_func(f) {
		}

		bool equal(Class* pThis, Function f){
			return pThis == m_this && f == m_func;
		}
		bool equal(Class* pThis){
			return pThis == m_this;
		}
		virtual slot_type type() override {
			return slot_method;
		}
		virtual Return invoke(Args... args) override {
			return (m_this->*m_func)();
		}
		virtual void  release() override {
			delete this;
		}

	private:
		Function  m_func;
		Class* m_this;
	};


    // 信号、事件封装。
	// 为了使代码更简洁，暂不去支持返回值模板。所有的回调函数都用void返回值
	// 支持多连接的版本multi connections
    template<typename... Args>
	class signal_mc
    {
    public:
		~signal_mc()
        {
			disconnect_all();
        }

        // 提交
		void emit(Args... args)
		{
			auto iter = m_connections.begin();
			decltype(iter) iterNext; 

			for (; iter != m_connections.end(); )
			{
				// 避免在回调中移除自己
				iterNext = iter;
				++iterNext;

				(*iter)->invoke(args...);

				iter = iterNext;
			}
		}

        // callback，在每次通过后，判断是否需要执行下一个连接。
        // callback 返回false，立即中断执行，返回true继续执行下一个连接
        void emit_foreach(std::function<bool()> callback, Args... args)
        {
            auto iter = m_connections.begin();
            decltype(iter) iterNext;

            for (; iter != m_connections.end();)
            {
                // 避免在回调中移除自己
                iterNext = iter;
                ++iterNext;

                (*iter)->invoke(args...);
                bool bContinue = callback();
                if (!bContinue)
                    return;

                iter = iterNext;
            }
        }

		
		/* 暂不去考虑返回值
		Return emit_return_last(Args... args)
        {
			Return ret;
            for (auto i : m_connections)
            {
				ret = i->invoke(args...);
            }

			return ret;
        }
		*/

		typedef void Return;  // 只去支持void返回值

        // 连接一个成员函数
        template<typename Class>
        void connect(Class* pThis, Return(Class::*pFunc)(Args...))
        {
            slot_base<Return, Args...>* p = new method_slot<Return, Class, Args...>(pThis, pFunc);
            m_connections.push_back(p);
        }

        // 连接一个全局函数
        void connect(Return(*pFunc)(Args...))
        {
            slot_base<Return, Args...>* p = new function_slot<Return, Args...>(pFunc);
            m_connections.push_back(p);
        }

		// 断开一个成员函数
		template<typename Class>
		void disconnect(Class* pThis, Return(Class::*pFunc)(Args...))
		{
			auto iter = m_connections.begin();
			for (; iter != m_connections.end(); ++iter)
			{
				auto slot = *iter;
				if (slot->type() != slot_method)
					continue;

				if (static_cast<method_slot<Return, Class, Args...>*>(slot)
						->equal(pThis, pFunc))
				{
                    (*iter)->release();
					m_connections.erase(iter);
					break;
				}
			}
		}

		// 断开一个对象下的所有连接
		template<typename Class>
		void disconnect(Class* pThis)
		{
			auto iter = m_connections.begin();
			for (; iter != m_connections.end();)
			{
				auto slot = *iter;
				if (slot->type() == slot_method)
				{
					if (static_cast<method_slot<Return, Class, Args...>*>(slot)
						->equal(pThis))
					{
                        (*iter)->release();
						iter = m_connections.erase(iter);
						continue;
					}
				}

				++iter;
			}
		}

		// 断开一个全局函数
		void disconnect(Return(*pFunc)(Args...))
		{
			auto iter = m_connections.begin();
			for (; iter != m_connections.end(); ++iter)
			{
				auto slot = *iter;
				if (slot->type() != slot_function)
					continue;

				if (static_cast<function_slot<Return, Args...>*>(slot)
						->equal(pFunc))
				{
                    (*iter)->release();
					m_connections.erase(iter);
					break;
				}
			}
		}

        void disconnect_all()
        {
            for (auto i : m_connections)
            {
                i->release();  // 交给创建模块去释放
            }
            m_connections.clear();
        }

    protected:
        std::list<slot_base<Return, Args...>*>  m_connections;
    };

	// 单连接版本，不支持一个事件有多个连接，用于减小一个signal成员的内存大小。
	// 不支持返回值，这样在书写信号量里，不用在模板中多一个void参数。
	// 为了便于回调，在这时在允许绑定的函数不提供参数列表，使用connect0来绑定。
	// 目前使用变长模板还未实现省略类似于QT任意数量参数的功能，仅支持无参数形式。

    // 单连接，支持返回值
	template<typename Return, typename... Args>
	class signal_r
	{
	public:
		~signal_r()
		{
			disconnect();
		}

		// 提交
		Return emit(Args... args)
		{
			if (m_connection)
				return m_connection->invoke(args...);

			return (Return());
		}

		// 连接一个成员函数
		template<typename Class>
		void connect(Class* pThis, Return(Class::*pFunc)(Args...))
		{
#ifdef _DEBUG
			if (m_connection)
				DebugBreak();
#endif
			disconnect();
			slot_base<Return, Args...>* p = new method_slot<Return, Class, Args...>(pThis, pFunc);
			m_connection = p;
		}
		template<typename Class>
		void connect0(Class* pThis, Return(Class::*pFunc)())
		{
#ifdef _DEBUG
			if (m_connection)
				DebugBreak();
#endif
			disconnect();
			m_connection = new method_slot0<Return, Class, Args...>(pThis, pFunc);
		}

		// 连接一个全局函数
		void connect(Return(*pFunc)(Args...))
		{
#ifdef _DEBUG
			if (m_connection)
				DebugBreak();
#endif
			disconnect();
			slot_base<Return, Args...>* p = new function_slot<Return, Args...>(pFunc);
			m_connection = p;
		}
		void connect0(Return(*pFunc)())
		{
#ifdef _DEBUG
			if (m_connection)
				DebugBreak();
#endif
			disconnect();
			m_connection = new function_slot0<Return, Args...>(pFunc);
		}

		void disconnect()
		{
			if (m_connection)
			{
				m_connection->release();
				m_connection = nullptr;
			}
		}

	protected:
		slot_base<Return, Args...>*  m_connection = nullptr;
	};

    // 单连接，无返回值
    template<typename... Args>
    class signal : public signal_r<void, Args...>
    {
    };

#else  // VS2013以下的版本，给每种参数个数封装一个类

template<typename Return, typename Args>
    class slot_base
    {
    public:
		virtual slot_type type() = 0;
        virtual Return invoke(Args args) = 0;
        virtual void  release() = 0;
    };

    // 全局函数封装
    template<typename Return, typename Args>
    class function_slot : public slot_base<Return, Args>
    {
    public:
        typedef Return(*Function)(Args);
		function_slot(Function f) : m_func(f) {
		}

		bool equal(Function f){
			return f == m_func;
		}
		virtual slot_type type() override { 
			return slot_function; 
		}
        virtual Return invoke(Args args) override{
			return m_func(args);
        }
        virtual void  release() override {
            delete this;
        }

    private:
		Function m_func;
    };

    // 成员函数封装
    template<typename Return, typename Class, typename Args>
    class method_slot : public slot_base<Return, Args>
    {
    public:
        typedef Return(Class::*Function)(Args);

        method_slot(Class* pThis, Function f) : 
                m_this(pThis), m_func(f) {
		}

		bool equal(Class* pThis, Function f){
			return pThis == m_this && f == m_func;
		}
		bool equal(Class* pThis){
			return pThis == m_this;
		}
		virtual slot_type type() override { 
			return slot_method; 
		}
        virtual Return invoke(Args args) override {
            return (m_this->*m_func)(args);
        }
        virtual void  release() override {
            delete this;
        }

    private:
        Function  m_func;
        Class* m_this;
    };

	// 单连接版本，不支持一个事件有多个连接，用于减小一个signal成员的内存大小。
	template</*typename Return, */typename Args>
	class signal
	{
	public:
		signal()
		{
			m_connection = nullptr;
		}
		~signal()
		{
			disconnect();
		}

		// 提交
		void emit(Args args)
		{
			if (m_connection)
				m_connection->invoke(args);

		}

		typedef void Return;  // 只去支持void返回值

		// 连接一个成员函数
		template<typename Class>
		void connect(Class* pThis, Return(Class::*pFunc)(Args))
		{
#ifdef _DEBUG
			if (m_connection)
			{
				DebugBreak();
			}
#endif
			disconnect();
			slot_base<Return, Args>* p = new method_slot<Return, Class, Args>(pThis, pFunc);
			m_connection = p;
		}

		// 连接一个全局函数
		void connect(Return(*pFunc)(Args))
		{
	#ifdef _DEBUG
			if (m_connection)
			{
				DebugBreak();
			}
	#endif
			disconnect();
			slot_base<Return, Args>* p = new function_slot<Return, Args>(pFunc);
			m_connection = p;
		}

		void disconnect()
		{
			if (m_connection)
			{
				m_connection->release();
				m_connection = nullptr;
			}
		}

	protected:
		slot_base<Return, Args>*  m_connection;
	};
#endif
}

#endif