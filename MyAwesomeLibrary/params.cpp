#include "params.h"
#include <algorithm>
#include <thread>
#include "miscUtils.h"
#include "../Whisper/API/iContext.cl.h"

whisper_params::whisper_params()
{
	constexpr unsigned int default_threads = 2;
	constexpr unsigned int max_threads = 4;

#ifdef _DEBUG
	n_threads = default_threads;
#else
	unsigned int hardware_concurrency = static_cast<unsigned int>(std::thread::hardware_concurrency());
	if (hardware_concurrency > max_threads || hardware_concurrency == 0) {
		n_threads = max_threads;
	}
	else {
		n_threads = hardware_concurrency;
	}
#endif
}

void whisper_print_usage(int argc, wchar_t** argv, const whisper_params& params)
{

}

static void __stdcall pfnListAdapter(const wchar_t* name, void*)
{
	wprintf(L"\"%s\"\n", name);
}

static void listGpus()
{
	printf("    Available graphic adapters:\n");
	HRESULT hr = Whisper::listGPUs(&pfnListAdapter, nullptr);
	if (SUCCEEDED(hr))
		return;
	printError("Unable to enumerate GPUs", hr);
}

bool whisper_params::parse(int argc, wchar_t* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		std::wstring arg = argv[i];

		if (arg[0] != '-')
		{
			fname_inp.push_back(arg);
			continue;
		}

		if (arg == L"-h" || arg == L"--help")
		{
			whisper_print_usage(argc, argv, *this);
			return false;
		}

		if (arg == L"-la" || arg == L"--list-adapters")
		{
			listGpus();
			return false;
		}

		else if (arg == L"-t" || arg == L"--threads") { n_threads = std::stoul(argv[++i]); }
		else if (arg == L"-p" || arg == L"--processors") { n_processors = std::stoul(argv[++i]); }
		else if (arg == L"-ot" || arg == L"--offset-t") { offset_t_ms = std::stoul(argv[++i]); }
		else if (arg == L"-on" || arg == L"--offset-n") { offset_n = std::stoul(argv[++i]); }
		else if (arg == L"-d" || arg == L"--duration") { duration_ms = std::stoul(argv[++i]); }
		else if (arg == L"-mc" || arg == L"--max-context") { max_context = std::stoul(argv[++i]); }
		else if (arg == L"-ml" || arg == L"--max-len") { max_len = std::stoul(argv[++i]); }
		else if (arg == L"-wt" || arg == L"--word-thold") { word_thold = std::stof(argv[++i]); }
		else if (arg == L"-su" || arg == L"--speed-up") { speed_up = true; }
		else if (arg == L"-tr" || arg == L"--translate") { translate = true; }
		else if (arg == L"-di" || arg == L"--diarize") { diarize = true; }
		else if (arg == L"-otxt" || arg == L"--output-txt") { output_txt = true; }
		else if (arg == L"-ovtt" || arg == L"--output-vtt") { output_vtt = true; }
		else if (arg == L"-osrt" || arg == L"--output-srt") { output_srt = true; }
		else if (arg == L"-owts" || arg == L"--output-words") { output_wts = true; }
		else if (arg == L"-ps" || arg == L"--print-special") { print_special = true; }
		else if (arg == L"-nc" || arg == L"--no-colors") { print_colors = false; }
		else if (arg == L"-nt" || arg == L"--no-timestamps") { no_timestamps = true; }
		else if (arg == L"-l" || arg == L"--language") { language = utf8(argv[++i]); }
		else if (arg == L"-m" || arg == L"--model") { model = argv[++i]; }
		else if (arg == L"-f" || arg == L"--file") { fname_inp.push_back(argv[++i]); }
		else if (arg == L"-gpu" || arg == L"--use-gpu") { gpu = argv[++i]; }
		else if (arg == L"--prompt") { prompt = utf8(argv[++i]); }
		else
		{
			fprintf(stderr, "error: unknown argument: %S\n", arg.c_str());
			whisper_print_usage(argc, argv, *this);
			return false;
		}
	}
	return true;
}