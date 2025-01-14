#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "base.h"

namespace native
{
    namespace internal
    {
        class callback_object_base
        {
        public:
            callback_object_base(void* data)
                : data_(data)
            {
                //printf("callback_object_base(): %x\n", this);
            }
            virtual ~callback_object_base()
            {
                //printf("~callback_object_base(): %x\n", this);
            }

            void* get_data() { return data_; }

        private:
            void* data_;
        };

        // SCO: serialized callback object
        template<typename callback_t>
        class callback_object : public callback_object_base
        {
        public:
            callback_object(const callback_t& callback, void* data)
                : callback_object_base(data)
                , callback_(callback)
            {
                //printf("callback_object<>(): %x\n", this);
            }

            virtual ~callback_object()
            {
                //printf("~callback_object<>(): %x\n", this);
            }

        public:
            template<typename ...A>
            void invoke(A&& ... args)
            {
                callback_(std::forward<A>(args)...);
            }

        private:
            callback_t callback_;
        };
    }

    typedef std::shared_ptr<internal::callback_object_base> callback_object_ptr;

    class callbacks
    {
    public:
        callbacks(int max_callbacks)
            : lut_(max_callbacks)
        {
            //printf("callbacks(): %x\n", this);
        }
        ~callbacks()
        {
            //printf("~callbacks(): %x\n", this);
        }

        template<typename callback_t>
        static void store(void* target, int cid, const callback_t& callback, void* data=nullptr)
        {
            reinterpret_cast<callbacks*>(target)->lut_[cid] = callback_object_ptr(new internal::callback_object<callback_t>(callback, data));
        }

        template<typename callback_t>
        static void get_data(void* target, int cid)
        {
            return reinterpret_cast<callbacks*>(target)->lut_[cid]->get_data();
        }

        template<typename callback_t, typename ...A>
        static void invoke(void* target, int cid, A&& ... args)
        {
            auto x = dynamic_cast<internal::callback_object<callback_t>*>(reinterpret_cast<callbacks*>(target)->lut_[cid].get());
            assert(x);
            x->invoke(args...);
        }

    private:
        std::vector<callback_object_ptr> lut_;
    };
}

#endif
