#include <utility>
#include <math.h>

namespace elo
{
	template<int base, int scale>
	inline double cdf(double a, double b)
	{
		return 1.0 / (1.0 + pow((double)base, ((a - b) / (double)scale)));
	}

	inline double get_k(double rating)
	{
		if (rating < 2100)
		{
			return 32.0;
		}
		else if (rating < 2400)
		{
			return 24.0;
		}
		else
		{
			return 16.0;
		}
	}

	inline std::pair<double, double> Match(double rating_a, double rating_b, double k_a, double k_b, double result)
	{
		double player_a_delta = k_a * (result - cdf<10, 400>(rating_b, rating_a));
		double player_b_delta = k_b * ((1.0 - result) - cdf<10, 400>(rating_a, rating_b));
		return std::make_pair(player_a_delta, player_b_delta);
	}

	inline std::pair<double, double> Match(double rating_a, double rating_b, double result)
	{
		return Match(rating_a, rating_b, get_k(rating_a), get_k(rating_b), result);
	}

	inline double ExpectedOutcome(double rating_a, double rating_b)
	{
		return cdf<10, 400>(rating_b, rating_a);
	}
}
