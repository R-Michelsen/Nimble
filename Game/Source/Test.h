#pragma once

#include <Nimble.h>

class Test : public Nimble::Game
{
public:
	virtual void Init() override;
	virtual void Simulate(float deltaTime) override;

private:
	Nimble::Entity camera;
};

eastl::unique_ptr<Nimble::Game> Nimble::CreateGame()
{
	return eastl::make_unique<Test>();
}
