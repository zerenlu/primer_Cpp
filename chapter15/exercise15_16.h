#include"exercise15_15.h"
class Limit_quote :public Disc_quote {
public:
	Limit_quote() = default;
	Limit_quote(const std::string&, double, std::size_t, double);
	double net_price(std::size_t) const override;

private:
	std::size_t max_qty = 0;
	double discount = 0.0;
};

Limit_quote::Limit_quote(const std::string& book, double p, std::size_t n, double dis) :
	Disc_quote(book,p,n,dis) {}

double Limit_quote::net_price(std::size_t cnt) const
{
	if (cnt < max_qty) {
		return cnt * price * (1 - discount);
	}
	else {
		return cnt * price;
	}
}