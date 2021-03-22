// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: dc.capnp

#pragma once

#include <capnp/generated-header-support.h>
#include <kj/windows-sanity.h>

#if CAPNP_VERSION != 8000
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif


namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(d4d302581b177f3d);
enum class Vote_d4d302581b177f3d: uint16_t {
  YES,
  NO,
  TIMEOUT,
};
CAPNP_DECLARE_ENUM(Vote, d4d302581b177f3d);
CAPNP_DECLARE_SCHEMA(83e79a7db5f807de);
enum class Voting_83e79a7db5f807de: uint16_t {
  MAJORITY,
  CONSENSUS,
};
CAPNP_DECLARE_ENUM(Voting, 83e79a7db5f807de);
CAPNP_DECLARE_SCHEMA(a148363bdfa62809);
enum class Subject_a148363bdfa62809: uint16_t {
  VALUE,
  ACTION,
};
CAPNP_DECLARE_ENUM(Subject, a148363bdfa62809);
CAPNP_DECLARE_SCHEMA(dca384a0f4f51dc9);
CAPNP_DECLARE_SCHEMA(f2801db0c89e53db);
CAPNP_DECLARE_SCHEMA(86abc6e859e85c21);
CAPNP_DECLARE_SCHEMA(beef6d7a502eec86);

}  // namespace schemas
}  // namespace capnp

namespace riaps {
namespace groups {
namespace poll {

typedef ::capnp::schemas::Vote_d4d302581b177f3d Vote;

typedef ::capnp::schemas::Voting_83e79a7db5f807de Voting;

typedef ::capnp::schemas::Subject_a148363bdfa62809 Subject;

struct ReqForVote {
  ReqForVote() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(dca384a0f4f51dc9, 4, 2)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct RepToVote {
  RepToVote() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(f2801db0c89e53db, 1, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct AnnounceVote {
  AnnounceVote() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(86abc6e859e85c21, 1, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct GroupVote {
  GroupVote() = delete;

  class Reader;
  class Builder;
  class Pipeline;
  enum Which: uint16_t {
    RFV,
    RTC,
    ANN,
  };

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(beef6d7a502eec86, 1, 1)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

// =======================================================================================

class ReqForVote::Reader {
public:
  typedef ReqForVote Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasTopic() const;
  inline  ::capnp::Data::Reader getTopic() const;

  inline bool hasRfvId() const;
  inline  ::capnp::Text::Reader getRfvId() const;

  inline  ::riaps::groups::poll::Voting getKind() const;

  inline  ::riaps::groups::poll::Subject getSubject() const;

  inline double getRelease() const;

  inline double getStarted() const;

  inline double getTimeout() const;

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

class ReqForVote::Builder {
public:
  typedef ReqForVote Builds;

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

  inline bool hasTopic();
  inline  ::capnp::Data::Builder getTopic();
  inline void setTopic( ::capnp::Data::Reader value);
  inline  ::capnp::Data::Builder initTopic(unsigned int size);
  inline void adoptTopic(::capnp::Orphan< ::capnp::Data>&& value);
  inline ::capnp::Orphan< ::capnp::Data> disownTopic();

  inline bool hasRfvId();
  inline  ::capnp::Text::Builder getRfvId();
  inline void setRfvId( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initRfvId(unsigned int size);
  inline void adoptRfvId(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownRfvId();

  inline  ::riaps::groups::poll::Voting getKind();
  inline void setKind( ::riaps::groups::poll::Voting value);

  inline  ::riaps::groups::poll::Subject getSubject();
  inline void setSubject( ::riaps::groups::poll::Subject value);

  inline double getRelease();
  inline void setRelease(double value);

  inline double getStarted();
  inline void setStarted(double value);

  inline double getTimeout();
  inline void setTimeout(double value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class ReqForVote::Pipeline {
public:
  typedef ReqForVote Pipelines;

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

class RepToVote::Reader {
public:
  typedef RepToVote Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasRfvId() const;
  inline  ::capnp::Text::Reader getRfvId() const;

  inline  ::riaps::groups::poll::Vote getVote() const;

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

class RepToVote::Builder {
public:
  typedef RepToVote Builds;

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

  inline bool hasRfvId();
  inline  ::capnp::Text::Builder getRfvId();
  inline void setRfvId( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initRfvId(unsigned int size);
  inline void adoptRfvId(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownRfvId();

  inline  ::riaps::groups::poll::Vote getVote();
  inline void setVote( ::riaps::groups::poll::Vote value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class RepToVote::Pipeline {
public:
  typedef RepToVote Pipelines;

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

class AnnounceVote::Reader {
public:
  typedef AnnounceVote Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasRfvId() const;
  inline  ::capnp::Text::Reader getRfvId() const;

  inline  ::riaps::groups::poll::Vote getVote() const;

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

class AnnounceVote::Builder {
public:
  typedef AnnounceVote Builds;

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

  inline bool hasRfvId();
  inline  ::capnp::Text::Builder getRfvId();
  inline void setRfvId( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initRfvId(unsigned int size);
  inline void adoptRfvId(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownRfvId();

  inline  ::riaps::groups::poll::Vote getVote();
  inline void setVote( ::riaps::groups::poll::Vote value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class AnnounceVote::Pipeline {
public:
  typedef AnnounceVote Pipelines;

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

class GroupVote::Reader {
public:
  typedef GroupVote Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline Which which() const;
  inline bool isRfv() const;
  inline bool hasRfv() const;
  inline  ::riaps::groups::poll::ReqForVote::Reader getRfv() const;

  inline bool isRtc() const;
  inline bool hasRtc() const;
  inline  ::riaps::groups::poll::RepToVote::Reader getRtc() const;

  inline bool isAnn() const;
  inline bool hasAnn() const;
  inline  ::riaps::groups::poll::AnnounceVote::Reader getAnn() const;

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

class GroupVote::Builder {
public:
  typedef GroupVote Builds;

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

  inline Which which();
  inline bool isRfv();
  inline bool hasRfv();
  inline  ::riaps::groups::poll::ReqForVote::Builder getRfv();
  inline void setRfv( ::riaps::groups::poll::ReqForVote::Reader value);
  inline  ::riaps::groups::poll::ReqForVote::Builder initRfv();
  inline void adoptRfv(::capnp::Orphan< ::riaps::groups::poll::ReqForVote>&& value);
  inline ::capnp::Orphan< ::riaps::groups::poll::ReqForVote> disownRfv();

  inline bool isRtc();
  inline bool hasRtc();
  inline  ::riaps::groups::poll::RepToVote::Builder getRtc();
  inline void setRtc( ::riaps::groups::poll::RepToVote::Reader value);
  inline  ::riaps::groups::poll::RepToVote::Builder initRtc();
  inline void adoptRtc(::capnp::Orphan< ::riaps::groups::poll::RepToVote>&& value);
  inline ::capnp::Orphan< ::riaps::groups::poll::RepToVote> disownRtc();

  inline bool isAnn();
  inline bool hasAnn();
  inline  ::riaps::groups::poll::AnnounceVote::Builder getAnn();
  inline void setAnn( ::riaps::groups::poll::AnnounceVote::Reader value);
  inline  ::riaps::groups::poll::AnnounceVote::Builder initAnn();
  inline void adoptAnn(::capnp::Orphan< ::riaps::groups::poll::AnnounceVote>&& value);
  inline ::capnp::Orphan< ::riaps::groups::poll::AnnounceVote> disownAnn();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class GroupVote::Pipeline {
public:
  typedef GroupVote Pipelines;

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

inline bool ReqForVote::Reader::hasTopic() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool ReqForVote::Builder::hasTopic() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Data::Reader ReqForVote::Reader::getTopic() const {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::capnp::Data::Builder ReqForVote::Builder::getTopic() {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void ReqForVote::Builder::setTopic( ::capnp::Data::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Data>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::capnp::Data::Builder ReqForVote::Builder::initTopic(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), size);
}
inline void ReqForVote::Builder::adoptTopic(
    ::capnp::Orphan< ::capnp::Data>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Data>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Data> ReqForVote::Builder::disownTopic() {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline bool ReqForVote::Reader::hasRfvId() const {
  return !_reader.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS).isNull();
}
inline bool ReqForVote::Builder::hasRfvId() {
  return !_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader ReqForVote::Reader::getRfvId() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_reader.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder ReqForVote::Builder::getRfvId() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}
inline void ReqForVote::Builder::setRfvId( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder ReqForVote::Builder::initRfvId(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), size);
}
inline void ReqForVote::Builder::adoptRfvId(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> ReqForVote::Builder::disownRfvId() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}

inline  ::riaps::groups::poll::Voting ReqForVote::Reader::getKind() const {
  return _reader.getDataField< ::riaps::groups::poll::Voting>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline  ::riaps::groups::poll::Voting ReqForVote::Builder::getKind() {
  return _builder.getDataField< ::riaps::groups::poll::Voting>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline void ReqForVote::Builder::setKind( ::riaps::groups::poll::Voting value) {
  _builder.setDataField< ::riaps::groups::poll::Voting>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, value);
}

inline  ::riaps::groups::poll::Subject ReqForVote::Reader::getSubject() const {
  return _reader.getDataField< ::riaps::groups::poll::Subject>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS);
}

inline  ::riaps::groups::poll::Subject ReqForVote::Builder::getSubject() {
  return _builder.getDataField< ::riaps::groups::poll::Subject>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS);
}
inline void ReqForVote::Builder::setSubject( ::riaps::groups::poll::Subject value) {
  _builder.setDataField< ::riaps::groups::poll::Subject>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS, value);
}

inline double ReqForVote::Reader::getRelease() const {
  return _reader.getDataField<double>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS);
}

inline double ReqForVote::Builder::getRelease() {
  return _builder.getDataField<double>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS);
}
inline void ReqForVote::Builder::setRelease(double value) {
  _builder.setDataField<double>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS, value);
}

inline double ReqForVote::Reader::getStarted() const {
  return _reader.getDataField<double>(
      ::capnp::bounded<2>() * ::capnp::ELEMENTS);
}

inline double ReqForVote::Builder::getStarted() {
  return _builder.getDataField<double>(
      ::capnp::bounded<2>() * ::capnp::ELEMENTS);
}
inline void ReqForVote::Builder::setStarted(double value) {
  _builder.setDataField<double>(
      ::capnp::bounded<2>() * ::capnp::ELEMENTS, value);
}

inline double ReqForVote::Reader::getTimeout() const {
  return _reader.getDataField<double>(
      ::capnp::bounded<3>() * ::capnp::ELEMENTS);
}

inline double ReqForVote::Builder::getTimeout() {
  return _builder.getDataField<double>(
      ::capnp::bounded<3>() * ::capnp::ELEMENTS);
}
inline void ReqForVote::Builder::setTimeout(double value) {
  _builder.setDataField<double>(
      ::capnp::bounded<3>() * ::capnp::ELEMENTS, value);
}

inline bool RepToVote::Reader::hasRfvId() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool RepToVote::Builder::hasRfvId() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader RepToVote::Reader::getRfvId() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder RepToVote::Builder::getRfvId() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void RepToVote::Builder::setRfvId( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder RepToVote::Builder::initRfvId(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), size);
}
inline void RepToVote::Builder::adoptRfvId(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> RepToVote::Builder::disownRfvId() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline  ::riaps::groups::poll::Vote RepToVote::Reader::getVote() const {
  return _reader.getDataField< ::riaps::groups::poll::Vote>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline  ::riaps::groups::poll::Vote RepToVote::Builder::getVote() {
  return _builder.getDataField< ::riaps::groups::poll::Vote>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline void RepToVote::Builder::setVote( ::riaps::groups::poll::Vote value) {
  _builder.setDataField< ::riaps::groups::poll::Vote>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, value);
}

inline bool AnnounceVote::Reader::hasRfvId() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool AnnounceVote::Builder::hasRfvId() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader AnnounceVote::Reader::getRfvId() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder AnnounceVote::Builder::getRfvId() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void AnnounceVote::Builder::setRfvId( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder AnnounceVote::Builder::initRfvId(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), size);
}
inline void AnnounceVote::Builder::adoptRfvId(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> AnnounceVote::Builder::disownRfvId() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline  ::riaps::groups::poll::Vote AnnounceVote::Reader::getVote() const {
  return _reader.getDataField< ::riaps::groups::poll::Vote>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline  ::riaps::groups::poll::Vote AnnounceVote::Builder::getVote() {
  return _builder.getDataField< ::riaps::groups::poll::Vote>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline void AnnounceVote::Builder::setVote( ::riaps::groups::poll::Vote value) {
  _builder.setDataField< ::riaps::groups::poll::Vote>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, value);
}

inline  ::riaps::groups::poll::GroupVote::Which GroupVote::Reader::which() const {
  return _reader.getDataField<Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline  ::riaps::groups::poll::GroupVote::Which GroupVote::Builder::which() {
  return _builder.getDataField<Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline bool GroupVote::Reader::isRfv() const {
  return which() == GroupVote::RFV;
}
inline bool GroupVote::Builder::isRfv() {
  return which() == GroupVote::RFV;
}
inline bool GroupVote::Reader::hasRfv() const {
  if (which() != GroupVote::RFV) return false;
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool GroupVote::Builder::hasRfv() {
  if (which() != GroupVote::RFV) return false;
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::riaps::groups::poll::ReqForVote::Reader GroupVote::Reader::getRfv() const {
  KJ_IREQUIRE((which() == GroupVote::RFV),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::ReqForVote>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::riaps::groups::poll::ReqForVote::Builder GroupVote::Builder::getRfv() {
  KJ_IREQUIRE((which() == GroupVote::RFV),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::ReqForVote>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void GroupVote::Builder::setRfv( ::riaps::groups::poll::ReqForVote::Reader value) {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::RFV);
  ::capnp::_::PointerHelpers< ::riaps::groups::poll::ReqForVote>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::riaps::groups::poll::ReqForVote::Builder GroupVote::Builder::initRfv() {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::RFV);
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::ReqForVote>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void GroupVote::Builder::adoptRfv(
    ::capnp::Orphan< ::riaps::groups::poll::ReqForVote>&& value) {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::RFV);
  ::capnp::_::PointerHelpers< ::riaps::groups::poll::ReqForVote>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::riaps::groups::poll::ReqForVote> GroupVote::Builder::disownRfv() {
  KJ_IREQUIRE((which() == GroupVote::RFV),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::ReqForVote>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline bool GroupVote::Reader::isRtc() const {
  return which() == GroupVote::RTC;
}
inline bool GroupVote::Builder::isRtc() {
  return which() == GroupVote::RTC;
}
inline bool GroupVote::Reader::hasRtc() const {
  if (which() != GroupVote::RTC) return false;
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool GroupVote::Builder::hasRtc() {
  if (which() != GroupVote::RTC) return false;
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::riaps::groups::poll::RepToVote::Reader GroupVote::Reader::getRtc() const {
  KJ_IREQUIRE((which() == GroupVote::RTC),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::RepToVote>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::riaps::groups::poll::RepToVote::Builder GroupVote::Builder::getRtc() {
  KJ_IREQUIRE((which() == GroupVote::RTC),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::RepToVote>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void GroupVote::Builder::setRtc( ::riaps::groups::poll::RepToVote::Reader value) {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::RTC);
  ::capnp::_::PointerHelpers< ::riaps::groups::poll::RepToVote>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::riaps::groups::poll::RepToVote::Builder GroupVote::Builder::initRtc() {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::RTC);
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::RepToVote>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void GroupVote::Builder::adoptRtc(
    ::capnp::Orphan< ::riaps::groups::poll::RepToVote>&& value) {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::RTC);
  ::capnp::_::PointerHelpers< ::riaps::groups::poll::RepToVote>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::riaps::groups::poll::RepToVote> GroupVote::Builder::disownRtc() {
  KJ_IREQUIRE((which() == GroupVote::RTC),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::RepToVote>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline bool GroupVote::Reader::isAnn() const {
  return which() == GroupVote::ANN;
}
inline bool GroupVote::Builder::isAnn() {
  return which() == GroupVote::ANN;
}
inline bool GroupVote::Reader::hasAnn() const {
  if (which() != GroupVote::ANN) return false;
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool GroupVote::Builder::hasAnn() {
  if (which() != GroupVote::ANN) return false;
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::riaps::groups::poll::AnnounceVote::Reader GroupVote::Reader::getAnn() const {
  KJ_IREQUIRE((which() == GroupVote::ANN),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::AnnounceVote>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::riaps::groups::poll::AnnounceVote::Builder GroupVote::Builder::getAnn() {
  KJ_IREQUIRE((which() == GroupVote::ANN),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::AnnounceVote>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void GroupVote::Builder::setAnn( ::riaps::groups::poll::AnnounceVote::Reader value) {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::ANN);
  ::capnp::_::PointerHelpers< ::riaps::groups::poll::AnnounceVote>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::riaps::groups::poll::AnnounceVote::Builder GroupVote::Builder::initAnn() {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::ANN);
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::AnnounceVote>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void GroupVote::Builder::adoptAnn(
    ::capnp::Orphan< ::riaps::groups::poll::AnnounceVote>&& value) {
  _builder.setDataField<GroupVote::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, GroupVote::ANN);
  ::capnp::_::PointerHelpers< ::riaps::groups::poll::AnnounceVote>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::riaps::groups::poll::AnnounceVote> GroupVote::Builder::disownAnn() {
  KJ_IREQUIRE((which() == GroupVote::ANN),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::riaps::groups::poll::AnnounceVote>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

}  // namespace
}  // namespace
}  // namespace

