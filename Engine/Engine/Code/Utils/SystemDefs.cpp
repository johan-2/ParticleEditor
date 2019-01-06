#include "SystemDefs.h"
#include "JsonHelpers.h"
#include <fstream>

bool  PostProcessing::APPLY_BLOOM                  = true;
bool  PostProcessing::BLOOM_USE_TWO_PASS_BLUR      = true;
float PostProcessing::BLOOM_INTENSITY              = 1.0f;
float PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1 = 4.0f;
float PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2 = 8.0f;

PostProcessing::PostProcessing(){}
PostProcessing::~PostProcessing(){}

void PostProcessing::ReadSettings(const char* file)
{
	FILE* fp; fopen_s(&fp, file, "rb");
	char readBuffer[65536];
	rapidjson::FileReadStream inStream(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document d;
	d.ParseStream(inStream);
	fclose(fp);

	assert(d.IsObject());

	APPLY_BLOOM                  = JSON::ReadBool(d,  "APPLY_BLOOM");
	BLOOM_USE_TWO_PASS_BLUR      = JSON::ReadBool(d,  "BLOOM_USE_TWO_PASS_BLUR");
	BLOOM_INTENSITY              = JSON::ReadFloat(d, "BLOOM_INTENSITY");
	BLOOM_BLUR_SCALE_DOWN_PASS_1 = JSON::ReadFloat(d, "BLOOM_BLUR_SCALE_DOWN_PASS_1");
	BLOOM_BLUR_SCALE_DOWN_PASS_2 = JSON::ReadFloat(d, "BLOOM_BLUR_SCALE_DOWN_PASS_2");
}

void PostProcessing::WriteSettings(const char* file)
{
	// create string buffer and json writer
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	// start the write
	writer.StartObject();

	JSON::WriteBool(&writer,  "APPLY_BLOOM",                  APPLY_BLOOM);
	JSON::WriteBool(&writer,  "BLOOM_USE_TWO_PASS_BLUR",      BLOOM_USE_TWO_PASS_BLUR);
	JSON::WriteFloat(&writer, "BLOOM_INTENSITY",              BLOOM_INTENSITY);
	JSON::WriteFloat(&writer, "BLOOM_BLUR_SCALE_DOWN_PASS_1", BLOOM_BLUR_SCALE_DOWN_PASS_1);
	JSON::WriteFloat(&writer, "BLOOM_BLUR_SCALE_DOWN_PASS_2", BLOOM_BLUR_SCALE_DOWN_PASS_2);
	
	// end object
	writer.EndObject();

	// create output file stream and write
	// out the json stringBuilder object to the file
	std::ofstream of(file);
	of << sb.GetString();

	of.close();
}