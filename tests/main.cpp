// Tests
#include <fsal.h>
#include <spdlog/spdlog.h>
#include "doctest.h"


TEST_CASE("[fsal] Filepaths")
{
	using fs = fsal::FileSystem;
	using loc = fsal::Location;
	spdlog::info("RoamingAppData: {}", fs::GetSystemPath(loc::kStorageSynced).string());
	spdlog::info("LocalAppData: {}", fs::GetSystemPath(loc::kStorageLocal).string());
	spdlog::info("UserPictures: {}", fs::GetSystemPath(loc::kPictures).string());
	spdlog::info("UserMusic: {}", fs::GetSystemPath(loc::kMusic).string());
	spdlog::info("ProgramData: {}", fs::GetSystemPath(loc::kWin_ProgramData).string());
	spdlog::info("ProgramFiles: {}", fs::GetSystemPath(loc::kWin_ProgramFiles).string());
	spdlog::info("Temp: {}", fs::GetSystemPath(loc::kTemp).string());
}


TEST_CASE("[fsal] Filepath normalization")
{
		CHECK(fsal::NormalizePath("./test_folder/folder_inside/../folder_inside/./") == "test_folder/folder_inside/");
		CHECK(fsal::NormalizePath("test_folder/folder_inside/../folder_inside/") == "test_folder/folder_inside/");
		CHECK(fsal::NormalizePath("test_folder/../test_folder/./folder_inside/../folder_inside/") == "test_folder/folder_inside/");
		CHECK(fsal::NormalizePath("test_folder/folder_inside/../folder_inside/.") == "test_folder/folder_inside");
}

TEST_CASE("[fsal] ReadWriteToFile")
{
	fsal::FileSystem fs;
	fs.ClearSearchPaths().PushSearchPath("../");

	{
		fsal::File f = fs.Open(fsal::Location("test.txt", fsal::Location::kCurrentDirectory), fsal::kWrite);
				CHECK(f);
		f = std::string("asdasdasd");
	}
	{
		fsal::File f = fs.Open(fsal::Location("test.txt", fsal::Location::kCurrentDirectory), fsal::kRead);
				CHECK(f);
		std::string str = f;
				CHECK(str == std::string("asdasdasd"));
	}
}

TEST_CASE("[fsal] CreateZIP")
{
	fsal::FileSystem fs;
	fs.ClearSearchPaths().PushSearchPath("../");
	{
		auto zipfile = fs.Open("out_archive.zip", fsal::kWrite);
		fsal::ZipWriter zip(zipfile);
		CHECK(zipfile);

		zip.AddFile("CMakeLists.txt", fs.Open("CMakeLists.txt"));
		zip.CreateDirectory("tests");
		zip.AddFile("tests/main.cpp", fs.Open("tests/main.cpp"));
		zip.CreateDirectory("tests2");
	}
}

TEST_CASE("[fsal] MountZIP")
{
	fsal::FileSystem fs;
	fs.ClearSearchPaths().PushSearchPath("../");
	{
		auto zipfile = fs.Open("out_archive.zip", fsal::kRead, true);
		CHECK(zipfile);

		auto zip = fsal::OpenZipArchive(zipfile);
		fs.MountArchive(zip);

		fsal::File file = fs.Open("tests/main.cpp", fsal::kRead);
		CHECK(file);
		std::string str = file;
		CHECK(std::string(str.begin(), str.begin() + 8) == "// Tests");

		spdlog::info("ListDirectory tests");
		auto list = zip.ListDirectory("tests");
		for(auto s: list)
		{
			spdlog::info("\t{}", s.c_str());
		}
	}
}
