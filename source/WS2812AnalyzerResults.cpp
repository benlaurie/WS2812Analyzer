#include "WS2812AnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "WS2812Analyzer.h"
#include "WS2812AnalyzerSettings.h"
#include <iostream>
#include <fstream>

WS2812AnalyzerResults::WS2812AnalyzerResults(WS2812Analyzer* analyzer,
					     WS2812AnalyzerSettings* settings)
  : AnalyzerResults(),
    mSettings(settings),
    mAnalyzer(analyzer) {
}

WS2812AnalyzerResults::~WS2812AnalyzerResults() {
}

void WS2812AnalyzerResults::GenerateBubbleText(U64 frame_index,
					       Channel& channel,
					       DisplayBase display_base) {
    ClearResultStrings();
    Frame frame = GetFrame(frame_index);

    if (frame.mData1 == RESET_VALUE) {
	AddResultString("RESET");
    } else if (mSettings->mType == WS2812AnalyzerSettings::FRAME) {
	// do RGB
	char rgb[16];
	U64 grb = frame.mData1;
	snprintf(rgb, sizeof rgb, "%06X (%02X%02X%02X)", grb,
		 0xff & (grb >> 8), 0xff & (grb >> 16), 0xff & grb);
	AddResultString(rgb);
	snprintf(rgb, sizeof rgb, "%06X", grb);
	AddResultString(rgb);
    } else {
	AddResultString(frame.mData1 - frame.mStartingSampleInclusive >
		    frame.mEndingSampleInclusive - frame.mData1 ? "1" : "0");
    }
}

void WS2812AnalyzerResults::GenerateExportFile(const char* file,
					       DisplayBase display_base,
					       U32 export_type_user_id) {
  std::ofstream file_stream( file, std::ios::out );

  U64 trigger_sample = mAnalyzer->GetTriggerSample();
  U32 sample_rate = mAnalyzer->GetSampleRate();

  file_stream << "Time [s],Value" << std::endl;

  U64 num_frames = GetNumFrames();
  for (U32 i=0; i < num_frames; i++) {
    Frame frame = GetFrame(i);
		
    char time_str[128];
    AnalyzerHelpers::GetTimeString(frame.mStartingSampleInclusive,
				   trigger_sample, sample_rate, time_str, 128);

    char number_str[128];
    AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8,
				     number_str, 128);

    file_stream << time_str << "," << number_str << std::endl;

    if (UpdateExportProgressAndCheckForCancel( i, num_frames ) == true) {
      file_stream.close();
      return;
    }
  }

  file_stream.close();
}

void WS2812AnalyzerResults::GenerateFrameTabularText(U64 frame_index,
						   DisplayBase display_base ) {
  Frame frame = GetFrame(frame_index);
  ClearResultStrings();

  char number_str[128];
  AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str,
				   128 );
  AddResultString(number_str);
}

void WS2812AnalyzerResults::GeneratePacketTabularText(U64 packet_id,
						    DisplayBase display_base) {
  ClearResultStrings();
  AddResultString( "not supported" );
}

void WS2812AnalyzerResults::GenerateTransactionTabularText(U64 transaction_id,
						    DisplayBase display_base) {
  ClearResultStrings();
  AddResultString( "not supported" );
}
