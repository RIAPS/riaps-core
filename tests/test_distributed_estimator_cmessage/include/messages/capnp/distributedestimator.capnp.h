// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: distributedestimator.capnp

#ifndef CAPNP_INCLUDED_bfb633a11018ca1c_
#define CAPNP_INCLUDED_bfb633a11018ca1c_

#include <capnp/generated-header-support.h>

#if CAPNP_VERSION != 5003
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif


namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(901eabe0b06f4fa2);
CAPNP_DECLARE_SCHEMA(ede2412e0dd4c79f);
CAPNP_DECLARE_SCHEMA(85af38b1afa32e32);
CAPNP_DECLARE_SCHEMA(a2bfa6930feff039);

}  // namespace schemas
}  // namespace capnp


struct SensorReady {
  SensorReady() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(901eabe0b06f4fa2, 0, 1)
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
    CAPNP_DECLARE_STRUCT_HEADER(ede2412e0dd4c79f, 0, 1)
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
    CAPNP_DECLARE_STRUCT_HEADER(85af38b1afa32e32, 0, 1)
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
    CAPNP_DECLARE_STRUCT_HEADER(a2bfa6930feff039, 0, 2)
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

  inline bool hasMsg() const;
  inline  ::capnp::Text::Reader getMsg() const;

  inline bool hasStrings() const;
  inline  ::capnp::List<double>::Reader getStrings() const;

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

  inline bool hasMsg();
  inline  ::capnp::Text::Builder getMsg();
  inline void setMsg( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initMsg(unsigned int size);
  inline void adoptMsg(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownMsg();

  inline bool hasStrings();
  inline  ::capnp::List<double>::Builder getStrings();
  inline void setStrings( ::capnp::List<double>::Reader value);
  inline void setStrings(::kj::ArrayPtr<const double> value);
  inline  ::capnp::List<double>::Builder initStrings(unsigned int size);
  inline void adoptStrings(::capnp::Orphan< ::capnp::List<double>>&& value);
  inline ::capnp::Orphan< ::capnp::List<double>> disownStrings();

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

inline bool SensorValue::Reader::hasMsg() const {
  return !_reader.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline bool SensorValue::Builder::hasMsg() {
  return !_builder.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader SensorValue::Reader::getMsg() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _reader.getPointerField(0 * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder SensorValue::Builder::getMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}
inline void SensorValue::Builder::setMsg( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder SensorValue::Builder::initMsg(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(
      _builder.getPointerField(0 * ::capnp::POINTERS), size);
}
inline void SensorValue::Builder::adoptMsg(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(
      _builder.getPointerField(0 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> SensorValue::Builder::disownMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(
      _builder.getPointerField(0 * ::capnp::POINTERS));
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

inline bool Estimate::Reader::hasMsg() const {
  return !_reader.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline bool Estimate::Builder::hasMsg() {
  return !_builder.getPointerField(0 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader Estimate::Reader::getMsg() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _reader.getPointerField(0 * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder Estimate::Builder::getMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}
inline void Estimate::Builder::setMsg( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(
      _builder.getPointerField(0 * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder Estimate::Builder::initMsg(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(
      _builder.getPointerField(0 * ::capnp::POINTERS), size);
}
inline void Estimate::Builder::adoptMsg(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(
      _builder.getPointerField(0 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> Estimate::Builder::disownMsg() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(
      _builder.getPointerField(0 * ::capnp::POINTERS));
}

inline bool Estimate::Reader::hasStrings() const {
  return !_reader.getPointerField(1 * ::capnp::POINTERS).isNull();
}
inline bool Estimate::Builder::hasStrings() {
  return !_builder.getPointerField(1 * ::capnp::POINTERS).isNull();
}
inline  ::capnp::List<double>::Reader Estimate::Reader::getStrings() const {
  return ::capnp::_::PointerHelpers< ::capnp::List<double>>::get(
      _reader.getPointerField(1 * ::capnp::POINTERS));
}
inline  ::capnp::List<double>::Builder Estimate::Builder::getStrings() {
  return ::capnp::_::PointerHelpers< ::capnp::List<double>>::get(
      _builder.getPointerField(1 * ::capnp::POINTERS));
}
inline void Estimate::Builder::setStrings( ::capnp::List<double>::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::List<double>>::set(
      _builder.getPointerField(1 * ::capnp::POINTERS), value);
}
inline void Estimate::Builder::setStrings(::kj::ArrayPtr<const double> value) {
  ::capnp::_::PointerHelpers< ::capnp::List<double>>::set(
      _builder.getPointerField(1 * ::capnp::POINTERS), value);
}
inline  ::capnp::List<double>::Builder Estimate::Builder::initStrings(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::List<double>>::init(
      _builder.getPointerField(1 * ::capnp::POINTERS), size);
}
inline void Estimate::Builder::adoptStrings(
    ::capnp::Orphan< ::capnp::List<double>>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::List<double>>::adopt(
      _builder.getPointerField(1 * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::List<double>> Estimate::Builder::disownStrings() {
  return ::capnp::_::PointerHelpers< ::capnp::List<double>>::disown(
      _builder.getPointerField(1 * ::capnp::POINTERS));
}


#endif  // CAPNP_INCLUDED_bfb633a11018ca1c_
