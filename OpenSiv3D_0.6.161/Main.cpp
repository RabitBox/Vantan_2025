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
};

class Brock {
	const Size SIZE{ 40, 20 };

private:
	Rect _shape;

public:
	void draw() {
		_shape.stretched(-1).draw(HSV{ _shape.y - 40 });
	}

	Brock() : _shape(Rect{0,0,SIZE}) {}
	Brock(Rect shape) : _shape(shape) {}
	virtual ~Brock() {}

	void initPos(int x, int y) {
		_shape.x = x * SIZE.x;
		_shape.y = 60 + y * SIZE.y;
	}
};

class Ball final{
	const double SPEED = 480.0;

private:
	Circle _shape;
	Vec2 _velocity;

	friend class Paddle;

public:
	void draw() { _shape.draw(); }
	void update() {
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
		, _velocity(Vec2{0, -SPEED}) {
	}
	~Ball() {}
};

class Paddle final
	: private GameObject {
private:
	Rect _shape;

public:
	void draw() override { _shape.rounded(3).draw(); }
	void update() override {
		_shape.x = Cursor::Pos().x;
	}

	Paddle(Rect shape) : _shape(shape) {}
	~Paddle() {}

	void checkIntersects(Ball* ball) {
		if (_shape.intersects(ball->_shape)) {
			ball->_velocity = Vec2{
				(ball->_shape.x - _shape.center().x) * 10,
				-ball->_velocity.y
			}.setLength(ball->SPEED);
		}
	}
};

void BallUpdate(const Circle& ball, Vec2& ballVelocity);

void Main()
{
	Ball* pBall = new Ball( Circle{ 400, 400, 8 } );
	Paddle* pPaddle = new Paddle( Rect{ Arg::center(100, 500), 60, 10 } );
	Brock brocks[MAX];

	for (int y = 0; y < Y_COUNT; ++y) {
		for (int x = 0; x < X_COUNT; ++x) {
			int index = (y * X_COUNT) + x;
			brocks[index].initPos(x,y);
		}
	}

	// ブロックの配列 | Array of bricks
	//Rect bricks[MAX];

	/*for (int y = 0; y < Y_COUNT; ++y) {
		for (int x = 0; x < X_COUNT; ++x) {
			int index = (y * X_COUNT) + x;
			bricks[index] = Rect{
				x * BrickSize.x,
				60 + y * BrickSize.y,
				BrickSize
			};
		}
	}*/

	while (System::Update())
	{

		// ブロックを順にチェックする | Check bricks in sequence
		/*for (int index = 0; index < sizeof(bricks) / sizeof(Rect); index++) {
			if (bricks[index].intersects(ball)) {
				if (bricks[index].bottom().intersects(ball) || bricks[index].top().intersects(ball))
				{
					ballVelocity.y *= -1;
				}
				else
				{
					ballVelocity.x *= -1;
				}
				bricks[index].y = -100;
				break;
			}
		}*/

		// パドルにあたったら | If the ball hits the left or right wall
		//if ((0 < ballVelocity.y) && paddle.intersects(ball))
		//{
		//	// パドルの中心からの距離に応じてはね返る方向（速度ベクトル）を変える | Change the direction (velocity vector) of the ball depending on the distance from the center of the paddle
		//	ballVelocity = Vec2{ (ball.x - paddle.center().x) * 10, -ballVelocity.y }.setLength(BallSpeedPerSec);
		//}

		pBall->update();
		pPaddle->update();

		pPaddle->checkIntersects( pBall );

		for (int i = 0; i < MAX; i++) {
			brocks[i].draw();
		}

		// マウスカーソルを非表示にする | Hide the mouse cursor
		Cursor::RequestStyle(CursorStyle::Hidden);

		pBall->draw();
		pPaddle->draw();
	}

	delete pBall;
	delete pPaddle;
}

void BallUpdate(const Circle& ball, Vec2& ballVelocity) {
	// 天井にぶつかったら | If the ball hits the ceiling
	if ((ball.y < 0) && (ballVelocity.y < 0))
	{
		// ボールの速度の Y 成分の符号を反転する | Reverse the sign of the Y component of the ball's velocity
		ballVelocity.y *= -1;
	}

	// 左右の壁にぶつかったら | If the ball hits the left or right wall
	if (((ball.x < 0) && (ballVelocity.x < 0))
		|| ((Scene::Width() < ball.x) && (0 < ballVelocity.x)))
	{
		// ボールの速度の X 成分の符号を反転する | Reverse the sign of the X component of the ball's velocity
		ballVelocity.x *= -1;
	}
}
