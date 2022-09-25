#pragma once
#include <sqlite3.h>
#include <stdexcept>

#include "Task.h"

namespace DB
{
	using callback = int(*)(void*, int, char**, char**);
	class API
	{
	private:
		sqlite3* database;
		callback setTaskId;

		void exec(const char*, callback call = nullptr, void* data = nullptr) const;
	public:
		API(const char* dbfile);

		void Insert(Task&) const;
		void UpdateStatus(const Task&) const;
		
		container SelectAllTasks() const;
		Task Select(long id) const;

		void Delete(long id) const;

		~API() { sqlite3_close(database); }
	};
}