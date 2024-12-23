AvroSchema::AvroSchema(std::string schema_json) iface(nullptr) {
    if (avro_schema_from_json_literal(std::string(schema_json.ToUTF8()), &schema) != 0) {
        throw std::runtime_error("Unable to parse schema: " + schema_json.ToStdString());
    }
    iface = avro_generic_class_from_schema(schema);
}

AvroSchema::~AvroSchema() {
    avro_value_iface_decref(iface);
    avro_schema_decref(schema);
}



AvroMemoryWriter::AvroMemoryWriter(AvroValue& value) value(value) {
    writer = avro_writer_memory(buffer, buffer_size);

    if (avro_value_write(writer, &value) != 0) {
        throw AvroException("Failed to serialize value");
    }
    buffer = avro_writer_get_buf(writer, &buffer_size);
}

AvroMemoryWriter::~AvroMemoryWriter() {
    avro_writer_free(writer);
}


AvroValueFromSchema::AvroValueFromSchema(AvroSchema &schema) schema(schema) {
    avro_generic_value_new(schema.iface, &value);
}

AvroValueFromSchema::~AvroValueFromSchema() {
    avro_value_decref(&value);
}


AvroValue::AvroValue() {
}

AvroValue::~AvroValue() {
}

std::string AvroValue::Serialize() {
    AvroMemoryWriter writer(*this);
    return std::string(writer.buffer, writer.buffer_size);
}

avro_type_t AvroValue::GetType() { return avro_value_get_type(&value); }
avro_schema_t AvroValue::GetSchema() { return avro_value_get_schema(&value); }
bool AvroValue::Equal(AvroValue other) { return avro_value_equal(&value, &other.value); }
uint32_t AvroValue::Hash() { return avro_value_hash(&value); }
void AvroValue::Reset() { if (avro_value_reset(value) != 0) throw AvroException("Failed to reset value"); }

bool AvroValue::GetBoolean() {
    int dest;
    if (avro_value_get_boolean(&value, &dest) != 0)
        throw AvroException("Failed to get bool value");
    return dest;
}
std::string AvroValue::GetBytes() {
    const void *dest, size_t size;
    if (avro_value_get_bytes(&value, &dest, &size) != 0)
        throw AvroException("Failed to get bytes value");
    return std::string(dest, size);
}
double AvroValue::GetDouble() {
    double dest;
    if (avro_value_get_double(&value, &dest) != 0)
        throw AvroException("Failed to get bytes value");
    return dest;
}
float AvroValue::GetFloat() {
    float dest;
    if (avro_value_get_float(&value, &dest) != 0)
        throw AvroException("Failed to get float value");
    return dest;
}
int32_t AvroValue::GetInt() {
    int32_t dest;
    if (avro_value_get_int(&value, &dest) != 0)
        throw AvroException("Failed to get int value");
    return dest;
}
int64_t AvroValue::GetLong() {
    int64_t dest;
    if (avro_value_get_long(&value, &dest) != 0)
        throw AvroException("Failed to get long value");
    return dest;
}
void AvroValue::GetNull() {
    if (avro_value_get_null(&value) != 0)
        throw AvroException("Failed to get null value");
}
std::string AvroValue::GetString() {
    const void *dest, size_t size;
    if (avro_value_get_string(&value, &dest, &size) != 0)
        throw AvroException("Failed to get bytes value");
    return std::string(dest, size);
}

int AvroValue::GetEnum() {
    int dest;
    if (avro_value_get_enum(&value, &dest) != 0)
        throw AvroException("Failed to get enum value");
    return dest;
}
std::string AvroValue::GetFixed() {
    const void *dest, size_t size;
    if (avro_value_get_fixed(&value, &dest, &size) != 0)
        throw AvroException("Failed to get bytes value");
    return std::string(dest, size);
}

void AvroValue::Set(bool src)         { if (avro_value_set_boolean(&value, (int) src) != 0) throw AvroException("Failed to set value"); }
void AvroValue::SetBytes(std::string src) { if (avro_value_set_bytes(&value, src.c_str(), src.size()) != 0) throw AvroException("Failed to set value"); }
void AvroValue::Set(double src)      { if (avro_value_set_double(&value, src) != 0) throw AvroException("Failed to set value"); }
void AvroValue::Set(float src)       { if (avro_value_set_float(&value, src) != 0) throw AvroException("Failed to set value"); }
void AvroValue::Set(int32_t src)     { if (avro_value_set_int(&value, src) != 0) throw AvroException("Failed to set value"); }
void AvroValue::Set(int64_t src)     { if (avro_value_set_long(&value, src) != 0) throw AvroException("Failed to set value"); }
void AvroValue::Set()                { if (avro_value_set_null(&value) != 0) throw AvroException("Failed to set value"); }
void AvroValue::Set(std::string src) { if (avro_value_set_string(&value, src.c_str()) != 0) throw AvroException("Failed to set value"); }
void AvroValue::SetEnum(int src)     { if (avro_value_set_enum(&value, src) != 0) throw AvroException("Failed to set value"); }
void AvroValue::SetFixed(std::string src) { if (avro_value_set_fixed(&value, src.c_str(), src.size()) != 0) throw AvroException("Failed to set value"); }


size_t AvroValue::GetSize() { size_t dest; if (avro_value_get_size(&value, &dest) != 0) throw AvroException("Failed to get size"); return dest; }
AvroValue AvroValue::Append(size_t *new_index) {
    AvroValue res;
    if (avro_value_append(&value, &res, new_index) != 0)
        throw AvroException("Failed to append value");
    return res;
}
AvroValue AvroValue::Get(std::string name, size_t *index) {
    AvroValue res;
    if (avro_value_get_by_name(&value, name.c_str(), &res, index) != 0)
        throw AvroException("Failed to get by name");
    return res;
}
AvroValue AvroValue::Get(size_t index) {
    AvroValue res;
    if (index < 0) { index = GetSize() - index; }
    if (avro_value_get_by_index(&value, index, &res.value, NULL) != 0)
        throw AvroException("Failed to get by index");
    return res;
}
AvroValue AvroValue::Get(size_t index, std::string& key) {
    AvroValue res;
    const char *keyres;
    if (index < 0) { index = GetSize() - index; }
    if (avro_value_get_by_index(&value, index, &res.value, &keyres) != 0)
        throw AvroException("Failed to get by index");
    key.assign(keyres);
    return res;
}
AvroValue AvroValue::Add(std::string name, size_t *index, int *is_new) {
    AvroValue res;
    if (avro_value_add(&value, name.c_str(), &res, index, is_new) != 0)
        throw AvroException("Failed to get by name");
    return res;
}

int AvroValue::GetDiscriminant() {
    int dest;
    if(avro_value_get_discriminant(&value, &dest) != 0)
        throw AvroException("Failed to get discriminant");
    return dest;
}
AvroValue AvroValue::Get() {
    AvroValue res;

    if (avro_value_get_current_branch(&value, &res.value) != 0)
        throw AvroException("Failed to get current branch");
    return res;
}
AvroValue AvroValue::SetCurrentBranch(int discriminant)
    AvroValue res;

    if (avro_value_set_branch(&value, discriminant, &res.value) != 0)
        throw AvroException("Failed to set current branch");
    return res;
}
