#pragma once
#include "Oquonie.h"
#include "Dialog.h"
#include <spdlog/spdlog.h>
#include <vector>
#include <string>
#include <stdio.h>


class Keyboard
{
	std::vector<std::string> m_locks;
	std::string m_history;

public:
	void ListenOnKeyDown(int keyCode)
	{
	}

	void ListenOnKeyUp(int keyCode)
	{
        if (!m_locks.empty())
        {
        	spdlog::info("Keyboard has locks:");
        	for (const auto& lock: m_locks)
	        {
        	    spdlog::info("\t{}", lock.c_str());
	        }

        	return;
        }

	    switch (keyCode)
	    {
	      case 87: KeyArrowUp(); break; // w
	      case 83: KeyArrowDown(); break; // S
	      case 65: KeyArrowLeft(); break; // A
	      case 68: KeyArrowRight(); break; // D

	      case 38: KeyArrowUp(); break;
	      case 40: KeyArrowDown(); break;
	      case 37: KeyArrowLeft(); break;
	      case 39: KeyArrowRight(); break;

	      case 27: KeyEscape(); break;
	      case 32: KeySpace(); break;
	    }

	    Record(keyCode);
	}

	void KeySpace()
	{
        spdlog::info("space");
    }

	void KeyArrowUp()
	{
        spdlog::info("arrow.up");
        Oquonie::GetInstance()->m_player->TryMove(0, 1);
    }

	void KeyArrowDown()
	{
        spdlog::info("arrow.down");
        Oquonie::GetInstance()->m_player->TryMove(0, -1);
    }

	void KeyArrowLeft()
	{
        spdlog::info("arrow.left");
        Oquonie::GetInstance()->m_player->TryMove(-1, 0);
    }

	void KeyArrowRight()
	{
        spdlog::info("arrow.right");
        Oquonie::GetInstance()->m_player->TryMove(1, 0);
    }

	void KeyEscape()
	{
        spdlog::info("escape");
        Oquonie::GetInstance()->m_dialog->Hide();
        //Oquonie::GetInstance()->overlay.hide()
    }

	void Lock(const std::string& lock_name)
	{
        spdlog::info("Added lock: {}", lock_name.c_str());
        m_locks.push_back(lock_name);
	}

	void UnLock(const std::string& lock_name)
	{
		bool unlocked = false;
        for (int i = 0; i < m_locks.size(); ++i)
        {
			if (m_locks[i] == lock_name)
			{
				m_locks.erase(m_locks.begin() + i);
				--i;
                spdlog::info("Unlocked: {}", lock_name.c_str());
                unlocked = true;
			}
        }
        if (!unlocked)
        {
            spdlog::info("No lock named: {}", lock_name.c_str());
        }
	}

	void Record(char key)
	{
//    if (this.history.length > 30) { this.history = this.history.substr(this.history.length - 30, 30) }
//
//    this.history += key
//
//    if (this.history.indexOf('noplacelikehome') > -1) {
//      this.history = ''
//      oquonie.stage.enter_room(1, 0, 0)
//    }
//    if (this.history.indexOf('susannakaysen') > -1) {
//      this.history = ''
//      oquonie.game.new()
//      oquonie.stage.enter_room(25, 0, 0)
//    }
//
//    if (this.history.indexOf('necomedre') > -1) {
//      this.history = ''
//      oquonie.player.transform('necomedre')
//    }
//    if (this.history.indexOf('nemedique') > -1) {
//      this.history = ''
//      oquonie.player.transform('nemedique')
//    }
//    if (this.history.indexOf('catfishbird') > -1) {
//      this.history = ''
//      oquonie.player.transform('catfishbird')
//    }
//    if (this.history.indexOf('redgate') > -1) {
//      this.history = ''
//      oquonie.stage.enter_room(100, 0, 0)
//    }
//    if (this.history.indexOf('speaker') > -1) {
//      this.history = ''
//      oquonie.music.pause_ambience()
//    }
//    if (this.history.indexOf('walkthrough') > -1) {
//      this.history = ''
//      oquonie.walkthrough.walk_all()
//    }
	}
};

