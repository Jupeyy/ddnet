#ifndef GAME_CLIENT_COMPONENTS_MATECAM_H
#define GAME_CLIENT_COMPONENTS_MATECAM_H

#include <game/client/component.h>

class CMateCam : public CComponent
{
public:
	int Sizeof() const override { return sizeof(*this); }

	void OnRender() override;
};

#endif // GAME_CLIENT_COMPONENTS_MATECAM_H
