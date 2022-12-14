#ifndef FLATBUFFERS_IDL_NAMER
#define FLATBUFFERS_IDL_NAMER

#include "flatbuffers/idl.h"
#include "namer.h"

namespace flatbuffers {

// Provides Namer capabilities to types defined in the flatbuffers IDL.
class IdlNamer : public Namer {
 public:
  explicit IdlNamer(Config config, std::set<std::string> keywords)
      : Namer(config, std::move(keywords)) {}

  using Namer::Constant;
  using Namer::Directories;
  using Namer::Field;
  using Namer::File;
  using Namer::Function;
  using Namer::Method;
  using Namer::Namespace;
  using Namer::NamespacedType;
  using Namer::ObjectType;
  using Namer::Type;
  using Namer::Variable;
  using Namer::Variant;

  // Types are always structs or enums so we can only expose these two
  // overloads.
  std::string Type(const StructDef &d) const { return Type(d.name); }
  std::string Type(const EnumDef &d) const { return Type(d.name); }

  std::string Function(const Definition &s) const { return Function(s.name); }

  std::string Field(const FieldDef &s) const { return Field(s.name); }

  std::string Variable(const FieldDef &s) const { return Variable(s.name); }

  std::string Variable(const StructDef &s) const { return Variable(s.name); }

  std::string Variant(const EnumVal &s) const { return Variant(s.name); }

  std::string EnumVariant(const EnumDef &e, const EnumVal &v) const {
    return Type(e) + config_.enum_variant_seperator + Variant(v);
  }

  std::string ObjectType(const StructDef &d) const {
    return ObjectType(d.name);
  }
  std::string ObjectType(const EnumDef &d) const { return ObjectType(d.name); }

  std::string Namespace(const struct Namespace &ns) const {
    return Namespace(ns.components);
  }

  std::string NamespacedType(const Definition &def) const {
    return NamespacedString(def.defined_namespace, Type(def.name));
  }

  std::string NamespacedObjectType(const Definition &def) const {
    return NamespacedString(def.defined_namespace, ObjectType(def.name));
  }

  std::string Directories(const struct Namespace &ns,
                          SkipDir skips = SkipDir::None) const {
    return Directories(ns.components, skips);
  }

  // Legacy fields do not really follow the usual config and should be
  // considered for deprecation.

  std::string LegacyRustNativeVariant(const EnumVal &v) const {
    return ConvertCase(EscapeKeyword(v.name), Case::kUpperCamel);
  }

  std::string LegacyRustFieldOffsetName(const FieldDef &field) const {
    return "VT_" + ConvertCase(EscapeKeyword(field.name), Case::kAllUpper);
  }

  // TODO(caspern): What's up with this case style?
  std::string LegacySwiftVariant(const EnumVal &ev) const {
    auto name = ev.name;
    if (isupper(name.front())) {
      std::transform(name.begin(), name.end(), name.begin(), CharToLower);
    }
    return EscapeKeyword(ConvertCase(name, Case::kLowerCamel));
  }

 private:
  std::string NamespacedString(const struct Namespace *ns,
                               const std::string &str) const {
    std::string ret;
    if (ns != nullptr) { ret += Namespace(ns->components); }
    if (!ret.empty()) ret += config_.namespace_seperator;
    return ret + str;
  }
};

// This is a temporary helper function for code generators to call until all
// flag-overriding logic into flatc.cpp
inline Namer::Config WithFlagOptions(const Namer::Config &input,
                                     const IDLOptions &opts,
                                     const std::string &path) {
  Namer::Config result = input;
  result.object_prefix = opts.object_prefix;
  result.object_suffix = opts.object_suffix;
  result.output_path = path;
  result.filename_suffix = opts.filename_suffix;
  return result;
}

}  // namespace flatbuffers

#endif  // FLATBUFFERS_IDL_NAMER