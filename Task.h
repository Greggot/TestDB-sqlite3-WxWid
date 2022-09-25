#pragma once
#include <string>
#include <cstdio>
#include <list>

enum state
{
    inQueue,
    inProcess,
    processError,
    finishError,
    finish
};

inline std::wstring getStateString(const state State)
{
    static const std::wstring states[] = {L"� �������", L"��� ������", L"��� ������",
        L"��������� � �������", L"���������"};
    return states[State];
}

struct Task
{
    long id;

	std::string text;
	time_t date;
    state status;

};
static bool operator==(const Task& r, const Task& l) {
    return r.id == l.id;
}

using container = std::list<Task>;

static const Task emptyTask{ 0 };