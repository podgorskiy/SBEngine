// Tests
#include "stack_vector.h"
#include "priority_queue.h"
#include <fsal.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include "doctest.h"
#include <map>


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

TEST_CASE("[stack] vector")
{
	stack::vector<glm::vec2, 17> v;

	CHECK(v.size() == 0);


	CHECK(sizeof(v) > 16 * sizeof(glm::vec2));

	spdlog::info("Vector size: {}", sizeof(v));

	v.push_back(glm::vec2(1.0f, 0.0f));
	v.push_back(glm::vec2(1.0f, 2.0f));

	CHECK(v[0] == glm::vec2(1.0f, 0.0f));
	CHECK(v[1] == glm::vec2(1.0f, 2.0f));

	v.push_back(glm::vec2(1.0f, 0.0f));
	v.push_back(glm::vec2(1.0f, 2.0f));

	v.clear();
	v.push_back(glm::vec2(1.0f, 2.0f));
	CHECK(v[0] == glm::vec2(1.0f, 2.0f));

	v.pop_back();
	v.push_back(glm::vec2(1.0f, 3.0f));
	CHECK(v[0] == glm::vec2(1.0f, 3.0f));

	for (int i = 0; i < 20; ++i)
	{
		v.push_back(glm::vec2(1.0f, 0.0f));
		CHECK(v.back() == glm::vec2(1.0f, 0.0f));

		v.push_back(glm::vec2(1.0f, 2.0f));
		CHECK(v.back() == glm::vec2(1.0f, 2.0f));
	}
}


TEST_CASE("[priority_queue] Basic")
{
	PriorityQueue<int> q;

	q.push(2);
	q.push(5);
	q.push(8);
	q.push(2);
	q.push(8);
	q.push(5);
	q.push(-5);
	q.push(4);
	q.push(10);

	int v[] = {-5, 2, 2, 4, 5, 5, 8, 8, 10};
	int i = 0;

	while(q.size() > 0)
	{
		int r = q.pop();
		spdlog::info("{}", r);
		CHECK_EQ(r, v[i++]);
	}
}


struct MyObject{
	int val;
	std::string name;
    bool operator < (const MyObject&  other) const
    {
        return this->val < other.val;
    }
};


struct MyObjectComp
{
	MyObjectComp(const std::vector<MyObject>& objects): objects(objects) {}

    bool operator()(int it1, int it2) const
    {
    	return objects[it1] < objects[it2];
    }
	const std::vector<MyObject>& objects;
};


TEST_CASE("[priority_queue] Indexed")
{
	std::vector<MyObject> example_registry;

	PriorityQueue<int, MyObjectComp> q(example_registry);

	example_registry.push_back({2, "2"}); q.push(example_registry.size() - 1);
	example_registry.push_back({5, "5"}); q.push(example_registry.size() - 1);
	example_registry.push_back({8, "8"}); q.push(example_registry.size() - 1);
	example_registry.push_back({2, "2"}); q.push(example_registry.size() - 1);
	example_registry.push_back({8, "8"}); q.push(example_registry.size() - 1);
	example_registry.push_back({5, "5"}); q.push(example_registry.size() - 1);
	example_registry.push_back({-5, "-5"}); q.push(example_registry.size() - 1);
	example_registry.push_back({4, "4"}); q.push(example_registry.size() - 1);
	example_registry.push_back({10, "10"}); q.push(example_registry.size() - 1);

	int v[] = {-5, 2, 2, 4, 5, 5, 8, 8, 10};
	int i = 0;

	while(q.size() > 0)
	{
		int id = q.pop();
		auto obj = example_registry[id];

		spdlog::info("{}", obj.name);
		CHECK_EQ(obj.val, v[i++]);
	}
}

