#include "DB.API.hpp"
using namespace DB;

API::API(const char* dbfile)
{
	if (sqlite3_open(dbfile, &database))
		throw std::exception("Cannot open database file");
	const char* sql =
		"CREATE TABLE \"Tasks\" (			\
		\"id\"	 INTEGER NOT NULL,		\
		\"date\" INTEGER NOT NULL,		\
		\"text\" TEXT NOT NULL,			\
		\"status\" INTEGER NOT NULL,	\
		PRIMARY KEY(\"id\" AUTOINCREMENT))";
	try
	{
		exec(sql);
	}
	catch (std::runtime_error)
	{
		// Tried to create already existing table
	}

	setTaskId = [](void* taskptr, int count, char** row, char**) {
		Task* task = (Task*)taskptr;
		task->id = std::atoi(*row);
		return 0;
	};
}

static int call(void* taskptr, int argc, char** row, char** column) 
{
	Task* task = (Task*)taskptr;
	for (int i = 0; i < argc; i++)
		printf("%s = %s\n", column[i], row[i] ? row[i] : "NULL");
	printf("\n");
	return 0;
}

void API::Insert(Task& task) const
{
	std::string date = std::to_string(task.date);
	std::string sql = "INSERT INTO Tasks (date, text, status) VALUES ("
		+ date + ", \'" + task.text + "\', " + std::to_string(task.status) + "); " \
		"SELECT (id) FROM Tasks WHERE date = " + date + ";";
	
	exec(sql.c_str(), setTaskId, &task);
}

void API::UpdateStatus(const Task& task) const
{
	std::string sql = "UPDATE Tasks set status = " + std::to_string(task.status) +
		" WHERE id = " + std::to_string(task.id);
	exec(sql.c_str());
}

container API::SelectAllTasks() const
{
	const char* sql = "SELECT * FROM Tasks";
	container tasks;
	exec(sql, [](void* contptr, int, char** row, char** column) {
		auto vector = (container*)contptr;
		Task task{ std::atoi(row[0]), row[2], std::atoi(row[1]), (state)std::atoi(row[3]) };
		vector->push_back(task);
		return 0;
	}, &tasks);
	return tasks;
}

Task API::Select(long id) const
{
	Task task;
	std::string sql = "SELECT * FROM Tasks WHERE id = " + std::to_string(id);
	exec(sql.c_str(), [](void* taskptr, int count, char** row, char** column) {
		Task* task = (Task*)taskptr;
		*task = Task{ std::atoi(row[0]), row[2], std::atoi(row[1]), (state)std::atoi(row[3]) };
		return 0;
	}, &task);
	return task;
}

void API::Delete(long id) const
{
	std::string sql = "DELETE FROM Tasks WHERE id = " + std::to_string(id);
	exec(sql.c_str());
}

void API::exec(const char* sql, callback call, void* data) const
{
	static char* error;
	if (sqlite3_exec(database, sql, call, data, &error))
		throw std::runtime_error(error);
}
