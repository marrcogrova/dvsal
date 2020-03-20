// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_IOHEADER_DV_H_
#define FLATBUFFERS_GENERATED_IOHEADER_DV_H_

#include <dv-sdk/data/cstring.hpp>
#include <dv-sdk/data/flatbuffers/flatbuffers.h>

namespace dv {

struct IOHeaderFlatbuffer;
struct IOHeader;

bool operator==(const IOHeader &lhs, const IOHeader &rhs);

inline const flatbuffers::TypeTable *IOHeaderTypeTable();

enum class Constants : int32_t { AEDAT_VERSION_LENGTH = 14, MIN = AEDAT_VERSION_LENGTH, MAX = AEDAT_VERSION_LENGTH };

inline const Constants (&EnumValuesConstants())[1] {
	static const Constants values[] = {Constants::AEDAT_VERSION_LENGTH};
	return values;
}

inline const char *const *EnumNamesConstants() {
	static const char *const names[] = {"AEDAT_VERSION_LENGTH", nullptr};
	return names;
}

inline const char *EnumNameConstants(Constants e) {
	if (e < Constants::AEDAT_VERSION_LENGTH || e > Constants::AEDAT_VERSION_LENGTH)
		return "";
	const size_t index = static_cast<int>(e) - static_cast<int>(Constants::AEDAT_VERSION_LENGTH);
	return EnumNamesConstants()[index];
}

enum class CompressionType : int32_t {
	NONE      = 0,
	LZ4       = 1,
	LZ4_HIGH  = 2,
	ZSTD      = 3,
	ZSTD_HIGH = 4,
	MIN       = NONE,
	MAX       = ZSTD_HIGH
};

inline const CompressionType (&EnumValuesCompressionType())[5] {
	static const CompressionType values[] = {CompressionType::NONE, CompressionType::LZ4, CompressionType::LZ4_HIGH,
		CompressionType::ZSTD, CompressionType::ZSTD_HIGH};
	return values;
}

inline const char *const *EnumNamesCompressionType() {
	static const char *const names[] = {"NONE", "LZ4", "LZ4_HIGH", "ZSTD", "ZSTD_HIGH", nullptr};
	return names;
}

inline const char *EnumNameCompressionType(CompressionType e) {
	if (e < CompressionType::NONE || e > CompressionType::ZSTD_HIGH)
		return "";
	const size_t index = static_cast<int>(e);
	return EnumNamesCompressionType()[index];
}

struct IOHeader : public flatbuffers::NativeTable {
	typedef IOHeaderFlatbuffer TableType;
	static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
		return "dv.IOHeader";
	}
	CompressionType compression;
	int64_t dataTablePosition;
	dv::cstring infoNode;
	IOHeader() : compression(CompressionType::NONE), dataTablePosition(-1) {
	}
	// Generated Constructor
	IOHeader(CompressionType _compression, int64_t _dataTablePosition, const dv::cstring &_infoNode) :
		compression{_compression}, dataTablePosition{_dataTablePosition}, infoNode{_infoNode} {
	}
};

inline bool operator==(const IOHeader &lhs, const IOHeader &rhs) {
	return (lhs.compression == rhs.compression) && (lhs.dataTablePosition == rhs.dataTablePosition)
		   && (lhs.infoNode == rhs.infoNode);
}

struct IOHeaderFlatbuffer FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
	typedef IOHeader NativeTableType;
	static FLATBUFFERS_CONSTEXPR const char *identifier = "IOHE";
	static const flatbuffers::TypeTable *MiniReflectTypeTable() {
		return IOHeaderTypeTable();
	}
	static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
		return "dv.IOHeader";
	}
	enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
		VT_COMPRESSION       = 4,
		VT_DATATABLEPOSITION = 6,
		VT_INFONODE          = 8
	};
	CompressionType compression() const {
		return static_cast<CompressionType>(GetField<int32_t>(VT_COMPRESSION, 0));
	}
	int64_t dataTablePosition() const {
		return GetField<int64_t>(VT_DATATABLEPOSITION, -1);
	}
	const flatbuffers::String *infoNode() const {
		return GetPointer<const flatbuffers::String *>(VT_INFONODE);
	}
	bool Verify(flatbuffers::Verifier &verifier) const {
		return VerifyTableStart(verifier) && VerifyField<int32_t>(verifier, VT_COMPRESSION)
			   && VerifyField<int64_t>(verifier, VT_DATATABLEPOSITION) && VerifyOffset(verifier, VT_INFONODE)
			   && verifier.VerifyString(infoNode()) && verifier.EndTable();
	}
	IOHeader *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
	void UnPackTo(IOHeader *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
	static void UnPackToFrom(
		IOHeader *_o, const IOHeaderFlatbuffer *_fb, const flatbuffers::resolver_function_t *_resolver = nullptr);
	static flatbuffers::Offset<IOHeaderFlatbuffer> Pack(flatbuffers::FlatBufferBuilder &_fbb, const IOHeader *_o,
		const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct IOHeaderBuilder {
	flatbuffers::FlatBufferBuilder &fbb_;
	flatbuffers::uoffset_t start_;
	void add_compression(CompressionType compression) {
		fbb_.AddElement<int32_t>(IOHeaderFlatbuffer::VT_COMPRESSION, static_cast<int32_t>(compression), 0);
	}
	void add_dataTablePosition(int64_t dataTablePosition) {
		fbb_.AddElement<int64_t>(IOHeaderFlatbuffer::VT_DATATABLEPOSITION, dataTablePosition, -1);
	}
	void add_infoNode(flatbuffers::Offset<flatbuffers::String> infoNode) {
		fbb_.AddOffset(IOHeaderFlatbuffer::VT_INFONODE, infoNode);
	}
	explicit IOHeaderBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) {
		start_ = fbb_.StartTable();
	}
	IOHeaderBuilder &operator=(const IOHeaderBuilder &);
	flatbuffers::Offset<IOHeaderFlatbuffer> Finish() {
		const auto end = fbb_.EndTable(start_);
		auto o         = flatbuffers::Offset<IOHeaderFlatbuffer>(end);
		return o;
	}
};

inline flatbuffers::Offset<IOHeaderFlatbuffer> CreateIOHeader(flatbuffers::FlatBufferBuilder &_fbb,
	CompressionType compression = CompressionType::NONE, int64_t dataTablePosition = -1,
	flatbuffers::Offset<flatbuffers::String> infoNode = 0) {
	IOHeaderBuilder builder_(_fbb);
	builder_.add_dataTablePosition(dataTablePosition);
	builder_.add_infoNode(infoNode);
	builder_.add_compression(compression);
	return builder_.Finish();
}

inline flatbuffers::Offset<IOHeaderFlatbuffer> CreateIOHeaderDirect(flatbuffers::FlatBufferBuilder &_fbb,
	CompressionType compression = CompressionType::NONE, int64_t dataTablePosition = -1,
	const char *infoNode = nullptr) {
	auto infoNode__ = infoNode ? _fbb.CreateString(infoNode) : 0;
	return dv::CreateIOHeader(_fbb, compression, dataTablePosition, infoNode__);
}

flatbuffers::Offset<IOHeaderFlatbuffer> CreateIOHeader(flatbuffers::FlatBufferBuilder &_fbb, const IOHeader *_o,
	const flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline IOHeader *IOHeaderFlatbuffer::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
	auto _o = new IOHeader();
	UnPackTo(_o, _resolver);
	return _o;
}

inline void IOHeaderFlatbuffer::UnPackTo(IOHeader *_o, const flatbuffers::resolver_function_t *_resolver) const {
	(void) _o;
	(void) _resolver;
	UnPackToFrom(_o, this, _resolver);
}

inline void IOHeaderFlatbuffer::UnPackToFrom(
	IOHeader *_o, const IOHeaderFlatbuffer *_fb, const flatbuffers::resolver_function_t *_resolver) {
	(void) _o;
	(void) _fb;
	(void) _resolver;
	{
		auto _e         = _fb->compression();
		_o->compression = _e;
	};
	{
		auto _e               = _fb->dataTablePosition();
		_o->dataTablePosition = _e;
	};
	{
		auto _e = _fb->infoNode();
		if (_e)
			_o->infoNode = dv::cstring(_e->c_str(), _e->size());
	};
}

inline flatbuffers::Offset<IOHeaderFlatbuffer> IOHeaderFlatbuffer::Pack(
	flatbuffers::FlatBufferBuilder &_fbb, const IOHeader *_o, const flatbuffers::rehasher_function_t *_rehasher) {
	return CreateIOHeader(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<IOHeaderFlatbuffer> CreateIOHeader(
	flatbuffers::FlatBufferBuilder &_fbb, const IOHeader *_o, const flatbuffers::rehasher_function_t *_rehasher) {
	(void) _rehasher;
	(void) _o;
	struct _VectorArgs {
		flatbuffers::FlatBufferBuilder *__fbb;
		const IOHeader *__o;
		const flatbuffers::rehasher_function_t *__rehasher;
	} _va = {&_fbb, _o, _rehasher};
	(void) _va;
	auto _compression       = _o->compression;
	auto _dataTablePosition = _o->dataTablePosition;
	auto _infoNode          = _o->infoNode.empty() ? 0 : _fbb.CreateString(_o->infoNode);
	return dv::CreateIOHeader(_fbb, _compression, _dataTablePosition, _infoNode);
}

inline const flatbuffers::TypeTable *ConstantsTypeTable() {
	static const flatbuffers::TypeCode type_codes[]    = {{flatbuffers::ET_INT, 0, 0}};
	static const flatbuffers::TypeFunction type_refs[] = {ConstantsTypeTable};
	static const int64_t values[]                      = {14};
	static const char *const names[]                   = {"AEDAT_VERSION_LENGTH"};
	static const flatbuffers::TypeTable tt = {flatbuffers::ST_ENUM, 1, type_codes, type_refs, values, names};
	return &tt;
}

inline const flatbuffers::TypeTable *CompressionTypeTypeTable() {
	static const flatbuffers::TypeCode type_codes[]    = {{flatbuffers::ET_INT, 0, 0}, {flatbuffers::ET_INT, 0, 0},
        {flatbuffers::ET_INT, 0, 0}, {flatbuffers::ET_INT, 0, 0}, {flatbuffers::ET_INT, 0, 0}};
	static const flatbuffers::TypeFunction type_refs[] = {CompressionTypeTypeTable};
	static const char *const names[]                   = {"NONE", "LZ4", "LZ4_HIGH", "ZSTD", "ZSTD_HIGH"};
	static const flatbuffers::TypeTable tt = {flatbuffers::ST_ENUM, 5, type_codes, type_refs, nullptr, names};
	return &tt;
}

inline const flatbuffers::TypeTable *IOHeaderTypeTable() {
	static const flatbuffers::TypeCode type_codes[]
		= {{flatbuffers::ET_INT, 0, 0}, {flatbuffers::ET_LONG, 0, -1}, {flatbuffers::ET_STRING, 0, -1}};
	static const flatbuffers::TypeFunction type_refs[] = {CompressionTypeTypeTable};
	static const char *const names[]                   = {"compression", "dataTablePosition", "infoNode"};
	static const flatbuffers::TypeTable tt = {flatbuffers::ST_TABLE, 3, type_codes, type_refs, nullptr, names};
	return &tt;
}

inline const dv::IOHeaderFlatbuffer *GetIOHeader(const void *buf) {
	return flatbuffers::GetRoot<dv::IOHeaderFlatbuffer>(buf);
}

inline const dv::IOHeaderFlatbuffer *GetSizePrefixedIOHeader(const void *buf) {
	return flatbuffers::GetSizePrefixedRoot<dv::IOHeaderFlatbuffer>(buf);
}

inline const char *IOHeaderIdentifier() {
	return "IOHE";
}

inline bool IOHeaderBufferHasIdentifier(const void *buf) {
	return flatbuffers::BufferHasIdentifier(buf, IOHeaderIdentifier());
}

inline bool VerifyIOHeaderBuffer(flatbuffers::Verifier &verifier) {
	return verifier.VerifyBuffer<dv::IOHeaderFlatbuffer>(IOHeaderIdentifier());
}

inline bool VerifySizePrefixedIOHeaderBuffer(flatbuffers::Verifier &verifier) {
	return verifier.VerifySizePrefixedBuffer<dv::IOHeaderFlatbuffer>(IOHeaderIdentifier());
}

inline void FinishIOHeaderBuffer(
	flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<dv::IOHeaderFlatbuffer> root) {
	fbb.Finish(root, IOHeaderIdentifier());
}

inline void FinishSizePrefixedIOHeaderBuffer(
	flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<dv::IOHeaderFlatbuffer> root) {
	fbb.FinishSizePrefixed(root, IOHeaderIdentifier());
}

inline std::unique_ptr<IOHeader> UnPackIOHeader(
	const void *buf, const flatbuffers::resolver_function_t *res = nullptr) {
	return std::unique_ptr<IOHeader>(GetIOHeader(buf)->UnPack(res));
}

} // namespace dv

#endif // FLATBUFFERS_GENERATED_IOHEADER_DV_H_
