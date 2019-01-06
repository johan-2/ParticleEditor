#include "SystemDefs.h"
#include "JsonHelpers.h"
#include <fstream>

bool  PostProcessing::APPLY_BLOOM                  = true;
bool  PostProcessing::BLOOM_USE_TWO_PASS_BLUR      = true;
float PostProcessing::BLOOM_INTENSITY              = 1.0f;
float PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1 = 4.0f;
float PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2 = 8.0f;

bool  PostProcessing::APPLY_DEPTH_OF_FIELD = true;
float PostProcessing::START_END_DOF_DST[]  = { 20.0f, 40.0f};

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

	APPLY_BLOOM                  = JSON::ReadBool(d,   "APPLY_BLOOM");
	BLOOM_USE_TWO_PASS_BLUR      = JSON::ReadBool(d,   "BLOOM_USE_TWO_PASS_BLUR");
	BLOOM_INTENSITY              = JSON::ReadFloat(d,  "BLOOM_INTENSITY");
	BLOOM_BLUR_SCALE_DOWN_PASS_1 = JSON::ReadFloat(d,  "BLOOM_BLUR_SCALE_DOWN_PASS_1");
	BLOOM_BLUR_SCALE_DOWN_PASS_2 = JSON::ReadFloat(d,  "BLOOM_BLUR_SCALE_DOWN_PASS_2");
	APPLY_DEPTH_OF_FIELD         = JSON::ReadBool(d,   "APPLY_DEPTH_OF_FIELD");
	XMFLOAT2 dofDst              = JSON::ReadFloat2(d, "START_END_DOF_DST");
	START_END_DOF_DST[0] = dofDst.x; START_END_DOF_DST[1] = dofDst.y;
}

void PostProcessing::WriteSettings(const char* file)
{
	// create string buffer and json writer
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	// start the write
	writer.StartObject();

	JSON::WriteBool(&writer,   "APPLY_BLOOM",                  APPLY_BLOOM);
	JSON::WriteBool(&writer,   "BLOOM_USE_TWO_PASS_BLUR",      BLOOM_USE_TWO_PASS_BLUR);
	JSON::WriteFloat(&writer,  "BLOOM_INTENSITY",              BLOOM_INTENSITY);
	JSON::WriteFloat(&writer,  "BLOOM_BLUR_SCALE_DOWN_PASS_1", BLOOM_BLUR_SCALE_DOWN_PASS_1);
	JSON::WriteFloat(&writer,  "BLOOM_BLUR_SCALE_DOWN_PASS_2", BLOOM_BLUR_SCALE_DOWN_PASS_2);
	JSON::WriteBool(&writer,   "APPLY_DEPTH_OF_FIELD",         APPLY_DEPTH_OF_FIELD);
	JSON::WriteFloat2(&writer, "START_END_DOF_DST",            XMFLOAT2(START_END_DOF_DST[0], START_END_DOF_DST[1]));
	
	// end object
	writer.EndObject();

	// create output file stream and write
	// out the json stringBuilder object to the file
	std::ofstream of(file);
	of << sb.GetString();

	of.close();
}