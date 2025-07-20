// Copyright [2025] TaCAD

#ifndef APP_COMMANDLINE_H_
#define APP_COMMANDLINE_H_

// stl includes
#include <iostream>
#include <string>

// cxxopts include
#include <cxxopts.hpp>

class CommandLine
{
public:
    CommandLine(int argc, char* argv[]);

    bool isSandboxEnabled() const
    {
        return mEnableSandbox;
    }
    bool isWelcomeDialogDisabled() const
    {
        return mNoWelcomeDialog;
    }
    bool hasPathToOpen() const
    {
        return !mPathToOpen.empty();
    }
    bool hasScriptToRun() const
    {
        return !mScriptToRun.empty();
    }

private:
    bool mEnableSandbox = false;      // Sandbox mode
    bool mNoWelcomeDialog = false;    // Welcome status
    std::string mPathToOpen;          // Path to open
    std::string mScriptToRun;         // Script to run

private:
    cxxopts::Options mOptions;
};

#endif  // APP_COMMANDLINE_H_
