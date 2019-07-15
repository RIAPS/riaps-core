#ifndef RIAPS_CORE_R_MESSAGEBUILDER_H
#define RIAPS_CORE_R_MESSAGEBUILDER_H

#include <capnp/message.h>
#include <capnp/serialize.h>

/**
 * Wraps capnp data object to build a new RIAPS message.
 * @tparam T
 */
template<typename T>
class MessageBuilder {
public:
    MessageBuilder() : spec_builder_(nullptr){
        spec_builder_ = capnp_builder_.initRoot<T>();
    };

    /**
     * Function call on the wrapped capnp object.
     */
    typename T::Builder* operator->() {
        return &spec_builder_;
    }

    typename T::Builder& spec_builder() {
        return spec_builder_;
    }

    /**
     * @return Capnp message array.
     */
    capnp::MallocMessageBuilder& capnp_builder() {
        return capnp_builder_;
    }

    ~MessageBuilder() = default;
private:
    capnp::MallocMessageBuilder capnp_builder_;
    typename T::Builder         spec_builder_;
};

#endif //RIAPS_CORE_R_MESSAGEBUILDER_H
