// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: activereplica.capnp

#ifndef CAPNP_INCLUDED_cda54ca269a5ad1a_
#define CAPNP_INCLUDED_cda54ca269a5ad1a_

#include <capnp/generated-header-support.h>

#if CAPNP_VERSION != 5003
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif


namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(8534110a723ca51b);
CAPNP_DECLARE_SCHEMA(fec3512b687a0ac4);
CAPNP_DECLARE_SCHEMA(d0e33accdab0608a);
CAPNP_DECLARE_SCHEMA(cb225cd542edaf7e);
CAPNP_DECLARE_SCHEMA(c12feccf2a7c6517);

}  // namespace schemas
}  // namespace capnp

namespace activereplica {
namespace messages {

struct SensorReady {
  SensorReady() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(8534110a723ca51b, 0, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

struct SensorValue {
  SensorValue() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(fec3512b687a0ac4, 1, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

struct SensorQuery {
  SensorQuery() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(d0e33accdab0608a, 0, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

struct Estimate {
  Estimate() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(cb225cd542edaf7e, 2, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

struct QueryRequest {
  QueryRequest() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(c12feccf2a7c6517, 1, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand = &schema->defaultBrand;
    #endif  // !CAPNP_LITE
  };
};

// =======================================================================================

class SensorReady::Reader {
public:
  typedef SensorReady Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline bool hasMsg() const;
  inline  ::capnp::Text::Reader getMsg() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class SensorReady::Builder {
public:
  typedef SensorReady Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasMsg();
  inline  ::capnp::Text::Builder getMsg();
  inline void setMsg( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initMsg(unsigned int size);
  inline void adoptMsg(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownMsg();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class SensorReady::Pipeline {
public:
  typedef SensorReady Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class SensorValue::Reader {
public:
  typedef SensorValue Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline double getValue() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class SensorValue::Builder {
public:
  typedef SensorValue Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline double getValue();
  inline void setValue(double value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class SensorValue::Pipeline {
public:
  typedef SensorValue Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class SensorQuery::Reader {
public:
  typedef SensorQuery Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline bool hasMsg() const;
  inline  ::capnp::Text::Reader getMsg() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class SensorQuery::Builder {
public:
  typedef SensorQuery Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasMsg();
  inline  ::capnp::Text::Builder getMsg();
  inline void setMsg( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initMsg(unsigned int size);
  inline void adoptMsg(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownMsg();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class SensorQuery::Pipeline {
public:
  typedef SensorQuery Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Estimate::Reader {
public:
  typedef Estimate Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline  ::uint32_t getId() const;

  inline double getValue() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Estimate::Builder {
public:
  typedef Estimate Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline  ::uint32_t getId();
  inline void setId( ::uint32_t value);

  inline double getValue();
  inline void setValue(double value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Estimate::Pipeline {
public:
  typedef Estimate Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class QueryRequest::Reader {
public:
  typedef QueryRequest Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand);
  }
#endif  // !CAPNP_LITE

  inline  ::uint32_t getId() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class QueryRequest::Builder {
public:
  typedef QueryRequest Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline  ::uint32_t getId();
  inline void setId( ::uint32_t value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class QueryRequest::Pipeline {
public:
  typedef QueryRequest Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

// =======================================================================================

inline bool SensorReady::Reader::hasMsg() const {
  return !_reader.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline bool SensorReady::Builder::hasMsg() {
  return !_builder.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader SensorReady::Reader::getMsg() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _reader.getPointerField(0 * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder SensorReady::Builder::getMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}
inline void SensorReady::Builder::setMsg( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder SensorReady::Builder::initMsg(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(
      _builder.getPointerField(0 * ::capnp::POINTERS), size);
}
inline void SensorReady::Builder::adoptMsg(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(
      _builder.getPointerField(0 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> SensorReady::Builder::disownMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}

inline double SensorValue::Reader::getValue() const {
  return _reader.getDataField<double>(
      0 * ::capnp::ELEMENTS);
}

inline double SensorValue::Builder::getValue() {
  return _builder.getDataField<double>(
      0 * ::capnp::ELEMENTS);
}
inline void SensorValue::Builder::setValue(double value) {
  _builder.setDataField<double>(
      0 * ::capnp::ELEMENTS, value);
}

inline bool SensorQuery::Reader::hasMsg() const {
  return !_reader.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline bool SensorQuery::Builder::hasMsg() {
  return !_builder.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader SensorQuery::Reader::getMsg() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _reader.getPointerField(0 * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder SensorQuery::Builder::getMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}
inline void SensorQuery::Builder::setMsg( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder SensorQuery::Builder::initMsg(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(
      _builder.getPointerField(0 * ::capnp::POINTERS), size);
}
inline void SensorQuery::Builder::adoptMsg(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(
      _builder.getPointerField(0 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> SensorQuery::Builder::disownMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}

inline  ::uint32_t Estimate::Reader::getId() const {
  return _reader.getDataField< ::uint32_t>(
      0 * ::capnp::ELEMENTS);
}

inline  ::uint32_t Estimate::Builder::getId() {
  return _builder.getDataField< ::uint32_t>(
      0 * ::capnp::ELEMENTS);
}
inline void Estimate::Builder::setId( ::uint32_t value) {
  _builder.setDataField< ::uint32_t>(
      0 * ::capnp::ELEMENTS, value);
}

inline double Estimate::Reader::getValue() const {
  return _reader.getDataField<double>(
      1 * ::capnp::ELEMENTS);
}

inline double Estimate::Builder::getValue() {
  return _builder.getDataField<double>(
      1 * ::capnp::ELEMENTS);
}
inline void Estimate::Builder::setValue(double value) {
  _builder.setDataField<double>(
      1 * ::capnp::ELEMENTS, value);
}

inline  ::uint32_t QueryRequest::Reader::getId() const {
  return _reader.getDataField< ::uint32_t>(
      0 * ::capnp::ELEMENTS);
}

inline  ::uint32_t QueryRequest::Builder::getId() {
  return _builder.getDataField< ::uint32_t>(
      0 * ::capnp::ELEMENTS);
}
inline void QueryRequest::Builder::setId( ::uint32_t value) {
  _builder.setDataField< ::uint32_t>(
      0 * ::capnp::ELEMENTS, value);
}

}  // namespace
}  // namespace

#endif  // CAPNP_INCLUDED_cda54ca269a5ad1a_
