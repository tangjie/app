#include "base/framework/observer_list.h"
#include "gtest/gtest.h"

class Observer {
public:
	Observer(int num) : num_(num) {
	}
	void set_num(int num) {
		num_ = num;
	}

	int num() {
		return num_;
	}

private:
	int num_;
};

TEST(ObserverListTest, Basic) {
	Observer a(1), b(2), c(3);
	EXPECT_EQ(1, a.num());
	EXPECT_EQ(2, b.num());
	EXPECT_EQ(3, c.num());
	std::shared_ptr<base::ObserverList<Observer>> observers(new base::ObserverList<Observer>());
	observers->AddObserver(&a);
	observers->AddObserver(&b);
	observers->AddObserver(&c);
	FOR_EACH_OBSERVER(Observer, observers, set_num(10));
	EXPECT_EQ(10, a.num());
	EXPECT_EQ(10, b.num());
	EXPECT_EQ(10, c.num());

}

// TODO: Add more test for observer list