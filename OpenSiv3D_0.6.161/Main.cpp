# include <Siv3D.hpp>

namespace {
	constexpr int Y_COUNT	= 5;
	constexpr int X_COUNT	= 20;
	constexpr int MAX		= Y_COUNT * X_COUNT;
}

class GameObject {
public:
	virtual void draw() = 0;
	virtual void update() = 0;

	virtual bool checkIntersects(GameObject* obj) { return false; }

	virtual ~GameObject() {}
};

class Ball final : public GameObject {
	static inline const double SPEED = 480.0;

private:
	Circle _shape;
	Vec2 _velocity;

	friend class Paddle;
	friend class Brock;

public:
	void draw() override { _shape.draw(); }
	void update() override {
		_shape.moveBy(_velocity * Scene::DeltaTime());

		if ((_shape.y < 0) && (_velocity.y < 0))
		{
			_velocity.y *= -1;
		}

		if (((_shape.x < 0) && (_velocity.x < 0))
			|| ((Scene::Width() < _shape.x) && (0 < _velocity.x)))
		{
			_velocity.x *= -1;
		}
	}

	Ball(Circle shape) :
		_shape(shape)
		, _velocity(Vec2{ 0, -SPEED }) {
	}
	~Ball() {}
};

class Brock : public GameObject {
	const Size SIZE{ 40, 20 };

private:
	Rect _shape;

public:
	void draw() override { _shape.stretched(-1).draw(HSV{ _shape.y - 40 }); }
	void update() override {}

	Brock() : _shape(Rect{0,0,SIZE}) {}
	Brock(Rect shape) : _shape(shape) {}
	virtual ~Brock() {}

	void initPos(int x, int y) {
		_shape.x = x * SIZE.x;
		_shape.y = 60 + y * SIZE.y;
	}

	bool checkIntersects(GameObject* obj) override {
		auto ball = (Ball*)obj;
		if (ball == nullptr) {
			return false;
		}

		if (_shape.intersects(ball->_shape)) {
			if (_shape.bottom().intersects(ball->_shape) || _shape.top().intersects(ball->_shape))
			{
				ball->_velocity.y *= -1;
			}
			else
			{
				ball->_velocity.x *= -1;
			}
			_shape.y = -100;
			return true;
		}
		return false;
	}
};

class Paddle final : public GameObject {
private:
	Rect _shape;

public:
	void draw() override { _shape.rounded(3).draw(); }
	void update() override {
		_shape.x = Cursor::Pos().x;
	}

	Paddle(Rect shape) : _shape(shape) {}
	~Paddle() {}

	bool checkIntersects(GameObject* obj) override {
		auto ball = (Ball*)obj;
		if ( ball == nullptr ) {
			return false;
		}

		if (_shape.intersects(ball->_shape)) {
			ball->_velocity = Vec2{
				(ball->_shape.x - _shape.center().x) * 10,
				-ball->_velocity.y
			}.setLength(ball->SPEED);

			return true;
		}

		return false;
	}
};

class GameScene {
	const int		kX;
	const int		kY;
	GameObject*		pBall;
	GameObject*		pPaddle;
	GameObject**	pBrocks;

public:
	void update() {
		pBall->update();
		pPaddle->update();

		pPaddle->checkIntersects(pBall);
		for (int index = 0; index < getMax(); index++) {
			if (pBrocks[index]->checkIntersects(pBall)) {
				break;
			}
		}
	}

	void draw() {
		for (int i = 0; i < getMax(); i++) {
			pBrocks[i]->draw();
		}
		pBall->draw();
		pPaddle->draw();
	}

	GameScene(int x, int y)
		: pBall(new Ball(Circle{ 400, 400, 8 }))
		, pPaddle(new Paddle(Rect{ Arg::center(100, 500), 60, 10 }))
		, kX(x), kY(y)
	{
		pBrocks = new GameObject * [kX * kY] { nullptr }; // 追加処理

		for (int y = 0; y < kY; ++y) {
			for (int x = 0; x < kX; ++x) {
				int index = (y * kX) + x;
				pBrocks[index] = new Brock();
				auto brock = (Brock*)pBrocks[index];
				if (brock) {
					brock->initPos(x, y);
				}
			}
		}
	}

	virtual ~GameScene() {
		delete pBall;
		delete pPaddle;
		for (int i = 0; i < getMax(); ++i) {
			delete pBrocks[i];
		}
		delete[] pBrocks;
	}

private:
	inline const int const getMax() const noexcept(true) { return kX * kY; };
};

void Main()
{
	GameScene scene(20, 5);

	while (System::Update())
	{
		scene.update();

		// マウスカーソルを非表示にする | Hide the mouse cursor
		Cursor::RequestStyle(CursorStyle::Hidden);

		scene.draw();
	}
}
