#include <stdio.h>
#include "spark.h"

void RgbWriteHeader(FILE *fp, int width, int height, int depth);
void RgbStage(SparkMemBufStruct input, SparkMemBufStruct stage, SparkInfoStruct spark_info);
void RgbResize(SparkMemBufStruct input, SparkMemBufStruct stage, SparkInfoStruct spark_info);
int RgbWriteSparkFrame(char *path, SparkMemBufStruct input, SparkMemBufStruct stage, SparkMemBufStruct resize, SparkInfoStruct spark_info);
