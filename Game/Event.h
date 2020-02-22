#pragma once
#include "Tile.h"
#include "Animator.h"
#include <string>


class Event;
typedef std::shared_ptr<Event> EventPtr;

class Animator;
typedef std::shared_ptr<Animator> AnimatorPtr;


class Event: public Tile, public Host
{
public:
	Event(const std::string& subtype): Tile("event"), m_location(0), m_name(subtype), m_state("idle"),
		m_is_mirrored(false)
	{
		m_animator = std::make_shared<Animator>(this);

		// TODO $(this.element).addClass(subtype)

	}
	virtual ~Event() = default;

	void MoveBy(int x, int y)
	{
	    m_x += x;
        m_y += y;

        auto p = PositionAt(m_x, m_y);
        int _y = p.first;
        int _x = p.second;

        // TODO $(this.element).finish();

	    auto target = m_animator;
	    target->SetState("walk.front");

	    if (x == 0 && y == -1 || x == -1 && y == 0) { target->SetState("walk.front"); }
	    if (x == 0 && y == 1 || x == 1 && y == 0) { target->SetState("walk.back"); }

	    // keyboard.lock("moving")
	    // setTimeout(function () { keyboard.unlock("moving") }, oquonie.speed * 0.5)
// TODO:
//	    $(this.element).animate({ left: _x, top: _y }, oquonie.speed, function () {
//	      if (x == 0 && y == -1 || x == -1 && y == 0) { target.set_state("idle.front") }
//	      if (x == 0 && y == 1 || x == 1 && y == 0) { target.set_state("idle.back") }
//	    })

	    // TODO: oquonie.stage.animate(this.x, this.y);
	}

	void MoveAt(int x, int y)
	{
	    m_x = x;
        m_y = y;

        auto p = PositionAt(m_x, m_y);
        int _y = p.first;
        int _x = p.second;

	    // TODO: $(this.element).css('top', _y).css('left', _x)
	}

	void StandByDoor(int x, int y)
	{
        // $(this.element).finish()
	    x = -x;
	    y = -y;
	    auto target = m_animator;
	    if (x == 0 && y == -1 || x == -1 && y == 0) { target->SetState("idle.front"); }
	    if (x == 0 && y == 1 || x == 1 && y == 0) { target->SetState("idle.back"); }

        target->Animate();
	}

	virtual bool IsCollider()
	{
		return false;
	}

	virtual bool ElicitsCollisionBump()
	{
		return true;
	}

	void Mirror()
	{
        m_is_mirrored = true;
        // TODO $(this.element).addClass("mirror")
	}

	/*
  this.bump_up = function (x, y) {
    let animator = this.animator
    if (x == 0 && y == -1 || x == -1 && y == 0) { animator.set_state('idle.front') }
    if (x == 0 && y == 1 || x == 1 && y == 0) { animator.set_state('idle.back') }

    $(this.element).finish()
    let origin_pos_y = parseInt(this.element.style.top)
    $(this.element).css('top', (origin_pos_y - 0.5) + '%').animate({ top: origin_pos_y + '%' }, oquonie.speed / 2)
  }

  this.bump_against = function (x, y) {
    let animator = this.animator
    if (x == 0 && y == -1 || x == -1 && y == 0) { animator.set_state('idle.front') }
    if (x == 0 && y == 1 || x == 1 && y == 0) { animator.set_state('idle.back') }

    let xSlant = x - y
    let ySlant = (-x - y) * 0.5

    $(this.element).finish()
    let origin_pos_x = parseInt(this.element.style.left)
    let origin_pos_y = parseInt(this.element.style.top)

    $(this.element).css('top', origin_pos_y + 0.5 * ySlant + '%').css('left', origin_pos_x + 0.5 * xSlant + '%')
    $(this.element).animate({ top: origin_pos_y + '%', left: origin_pos_x + '%' }, oquonie.speed / 2)
  }

  this.receive_bump = function () {
    $(this.element).finish()
    let origin_pos_y = parseInt(this.element.style.top)
    $(this.element).css('top', (origin_pos_y - 0.5) + '%').animate({ top: origin_pos_y + '%' }, oquonie.speed / 2)
  }
	 */

	virtual void OnCollision()
	{
        // console.log("On collision, no effect");
	}

	virtual void OnStep()
	{
        // console.log("On step, no effect");
	}

	virtual void OnSight()
	{
        // console.log("On sight, no effect");
	}

	virtual std::string GetHostHame()
	{
		return m_name;
	}

	virtual int GetHostID()
	{
		return m_id;
	}

	int m_location;
	AnimatorPtr m_animator;

private:
	std::string m_name;
	std::string m_state;
	bool m_is_mirrored;
};
