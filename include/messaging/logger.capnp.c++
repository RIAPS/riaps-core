// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: logger.capnp

#include "logger.capnp.h"

namespace capnp {
namespace schemas {
static const ::capnp::_::AlignedData<38> b_b2573c5b13e5bbea = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    234, 187, 229,  19,  91,  60,  87, 178,
     13,   0,   0,   0,   2,   0,   0,   0,
     32, 184,  18, 218, 153, 137, 160, 204,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 226,   0,   0,   0,
     33,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     29,   0,   0,   0, 127,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    108, 111, 103, 103, 101, 114,  46,  99,
     97, 112, 110, 112,  58,  76, 111, 103,
     77, 101, 115, 115,  97, 103, 101,  84,
    121, 112, 101,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
     20,   0,   0,   0,   1,   0,   2,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     53,   0,   0,   0,  50,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
     45,   0,   0,   0,  66,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      2,   0,   0,   0,   0,   0,   0,   0,
     37,   0,   0,   0,  58,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   0,   0,   0,   0,
     29,   0,   0,   0,  42,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      4,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0,  50,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    101, 114, 114, 111, 114,   0,   0,   0,
    119,  97, 114, 110, 105, 110, 103,   0,
    110, 111, 116, 105,  99, 101,   0,   0,
    105, 110, 102, 111,   0,   0,   0,   0,
    100, 101,  98, 117, 103,   0,   0,   0, }
};
::capnp::word const* const bp_b2573c5b13e5bbea = b_b2573c5b13e5bbea.words;
#if !CAPNP_LITE
static const uint16_t m_b2573c5b13e5bbea[] = {4, 0, 3, 2, 1};
const ::capnp::_::RawSchema s_b2573c5b13e5bbea = {
  0xb2573c5b13e5bbea, b_b2573c5b13e5bbea.words, 38, nullptr, m_b2573c5b13e5bbea,
  0, 5, nullptr, nullptr, nullptr, { &s_b2573c5b13e5bbea, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
CAPNP_DEFINE_ENUM(LogMessageType_b2573c5b13e5bbea, b2573c5b13e5bbea);
static const ::capnp::_::AlignedData<34> b_d648ccfd006b6169 = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    105,  97, 107,   0, 253, 204,  72, 214,
     13,   0,   0,   0,   1,   0,   0,   0,
     32, 184,  18, 218, 153, 137, 160, 204,
      1,   0,   7,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 234,   0,   0,   0,
     33,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     29,   0,   0,   0,  63,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    108, 111, 103, 103, 101, 114,  46,  99,
     97, 112, 110, 112,  58,  78, 101, 119,
     76, 111, 103,  80, 117,  98, 108, 105,
    115, 104, 101, 114,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
      4,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     13,   0,   0,   0,  74,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     12,   0,   0,   0,   3,   0,   1,   0,
     24,   0,   0,   0,   2,   0,   1,   0,
    101, 110, 100, 112, 111, 105, 110, 116,
      0,   0,   0,   0,   0,   0,   0,   0,
     12,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     12,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_d648ccfd006b6169 = b_d648ccfd006b6169.words;
#if !CAPNP_LITE
static const uint16_t m_d648ccfd006b6169[] = {0};
static const uint16_t i_d648ccfd006b6169[] = {0};
const ::capnp::_::RawSchema s_d648ccfd006b6169 = {
  0xd648ccfd006b6169, b_d648ccfd006b6169.words, 34, nullptr, m_d648ccfd006b6169,
  0, 1, i_d648ccfd006b6169, nullptr, nullptr, { &s_d648ccfd006b6169, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<48> b_eae806d29c552650 = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
     80,  38,  85, 156, 210,   6, 232, 234,
     13,   0,   0,   0,   1,   0,   1,   0,
     32, 184,  18, 218, 153, 137, 160, 204,
      1,   0,   7,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 194,   0,   0,   0,
     29,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0, 119,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    108, 111, 103, 103, 101, 114,  46,  99,
     97, 112, 110, 112,  58,  76, 111, 103,
     77, 101, 115, 115,  97, 103, 101,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
      8,   0,   0,   0,   3,   0,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     41,   0,   0,   0, 122,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     40,   0,   0,   0,   3,   0,   1,   0,
     52,   0,   0,   0,   2,   0,   1,   0,
      1,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     49,   0,   0,   0,  66,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     44,   0,   0,   0,   3,   0,   1,   0,
     56,   0,   0,   0,   2,   0,   1,   0,
    108, 111, 103,  77, 101, 115, 115,  97,
    103, 101,  84, 121, 112, 101,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
    234, 187, 229,  19,  91,  60,  87, 178,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    109, 101, 115, 115,  97, 103, 101,   0,
     12,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     12,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_eae806d29c552650 = b_eae806d29c552650.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_eae806d29c552650[] = {
  &s_b2573c5b13e5bbea,
};
static const uint16_t m_eae806d29c552650[] = {0, 1};
static const uint16_t i_eae806d29c552650[] = {0, 1};
const ::capnp::_::RawSchema s_eae806d29c552650 = {
  0xeae806d29c552650, b_eae806d29c552650.words, 48, d_eae806d29c552650, m_eae806d29c552650,
  1, 2, i_eae806d29c552650, nullptr, nullptr, { &s_eae806d29c552650, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
static const ::capnp::_::AlignedData<49> b_912e97ee2e6354fc = {
  {   0,   0,   0,   0,   5,   0,   6,   0,
    252,  84,  99,  46, 238, 151,  46, 145,
     13,   0,   0,   0,   1,   0,   1,   0,
     32, 184,  18, 218, 153, 137, 160, 204,
      1,   0,   7,   0,   0,   0,   2,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     21,   0,   0,   0, 138,   0,   0,   0,
     29,   0,   0,   0,   7,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     25,   0,   0,   0, 119,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    108, 111, 103, 103, 101, 114,  46,  99,
     97, 112, 110, 112,  58,  76, 111, 103,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,   1,   0,
      8,   0,   0,   0,   3,   0,   4,   0,
      0,   0, 255, 255,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     41,   0,   0,   0, 130,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     40,   0,   0,   0,   3,   0,   1,   0,
     52,   0,   0,   0,   2,   0,   1,   0,
      1,   0, 254, 255,   0,   0,   0,   0,
      0,   0,   1,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     49,   0,   0,   0,  90,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     48,   0,   0,   0,   3,   0,   1,   0,
     60,   0,   0,   0,   2,   0,   1,   0,
    110, 101, 119,  76, 111, 103,  80, 117,
     98, 108, 105, 115, 104, 101, 114,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
    105,  97, 107,   0, 253, 204,  72, 214,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    108, 111, 103,  77, 101, 115, 115,  97,
    103, 101,   0,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
     80,  38,  85, 156, 210,   6, 232, 234,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0, }
};
::capnp::word const* const bp_912e97ee2e6354fc = b_912e97ee2e6354fc.words;
#if !CAPNP_LITE
static const ::capnp::_::RawSchema* const d_912e97ee2e6354fc[] = {
  &s_d648ccfd006b6169,
  &s_eae806d29c552650,
};
static const uint16_t m_912e97ee2e6354fc[] = {1, 0};
static const uint16_t i_912e97ee2e6354fc[] = {0, 1};
const ::capnp::_::RawSchema s_912e97ee2e6354fc = {
  0x912e97ee2e6354fc, b_912e97ee2e6354fc.words, 49, d_912e97ee2e6354fc, m_912e97ee2e6354fc,
  2, 2, i_912e97ee2e6354fc, nullptr, nullptr, { &s_912e97ee2e6354fc, nullptr, nullptr, 0, 0, nullptr }
};
#endif  // !CAPNP_LITE
}  // namespace schemas
}  // namespace capnp

// =======================================================================================


// NewLogPublisher
#ifndef _MSC_VER
constexpr uint16_t NewLogPublisher::_capnpPrivate::dataWordSize;
constexpr uint16_t NewLogPublisher::_capnpPrivate::pointerCount;
#endif
#if !CAPNP_LITE
constexpr ::capnp::Kind NewLogPublisher::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* NewLogPublisher::_capnpPrivate::schema;
constexpr ::capnp::_::RawBrandedSchema const* NewLogPublisher::_capnpPrivate::brand;
#endif  // !CAPNP_LITE

// LogMessage
#ifndef _MSC_VER
constexpr uint16_t LogMessage::_capnpPrivate::dataWordSize;
constexpr uint16_t LogMessage::_capnpPrivate::pointerCount;
#endif
#if !CAPNP_LITE
constexpr ::capnp::Kind LogMessage::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* LogMessage::_capnpPrivate::schema;
constexpr ::capnp::_::RawBrandedSchema const* LogMessage::_capnpPrivate::brand;
#endif  // !CAPNP_LITE

// Log
#ifndef _MSC_VER
constexpr uint16_t Log::_capnpPrivate::dataWordSize;
constexpr uint16_t Log::_capnpPrivate::pointerCount;
#endif
#if !CAPNP_LITE
constexpr ::capnp::Kind Log::_capnpPrivate::kind;
constexpr ::capnp::_::RawSchema const* Log::_capnpPrivate::schema;
constexpr ::capnp::_::RawBrandedSchema const* Log::_capnpPrivate::brand;
#endif  // !CAPNP_LITE


