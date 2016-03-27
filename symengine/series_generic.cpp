#include <exception>
#include <algorithm>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x, unsigned int prec) {
    UnivariateExprPolynomial p(UnivariatePolynomial::create(symbol(x), {}));
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries> visitor(std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const {
    return p_.get_univariate_poly()->hash() + std::size_t(get_degree() * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const {
    SYMENGINE_ASSERT(is_a<UnivariateSeries>(other))
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return p_.get_basic()->__cmp__(*o.p_.get_basic());
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    umap_int_basic map;
    for (int i = 0; i <= get_degree(); i++)
       map[i] = p_.get_univariate_poly()->get_dict().at(i).get_basic();
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int deg) const {
    if (p_.get_univariate_poly()->get_dict().count(deg) == 0)
        return zero;
    else
        return p_.get_univariate_poly()->get_dict().at(deg).get_basic();
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s) {
    UnivariateExprPolynomial p(UnivariatePolynomial::create(symbol(s), {0, 1}));
    return p;
}

Expression UnivariateSeries::convert(const Basic &x) {
    return Expression(x.rcp_from_this());
}

RCP<const UnivariatePolynomial> UnivariateSeries::trunc_poly(const RCP<const Symbol> &var, const map_int_Expr &d, unsigned prec) {
    map_int_Expr dict_trunc;
    unsigned int max = 0;
    for (const auto &it : d) {
        if ((unsigned)it.first < prec) {
            if (max < (unsigned)it.first)
                max = (unsigned)it.first;
            dict_trunc[it.first] = it.second;
        }
    }
    return univariate_polynomial(var, max, std::move(dict_trunc));
}

unsigned UnivariateSeries::ldegree(const UnivariateExprPolynomial &s) {
    return s.get_univariate_poly()->get_dict().begin()->first;
}

UnivariateExprPolynomial UnivariateSeries::mul(const UnivariateExprPolynomial &a, const UnivariateExprPolynomial &b, unsigned prec) {
    map_int_Expr p;
    for (auto &it1 : a.get_univariate_poly()->get_dict()) {
        for (auto &it2 : b.get_univariate_poly()->get_dict()) {
            unsigned int exp = it1.first + it2.first;
            if (exp < prec) {
                p[exp] += it1.second * it2.second;
            } else {
                break;
            }
        }
    }
    return UnivariateExprPolynomial(UnivariatePolynomial::from_dict(a.get_univariate_poly()->get_var(), std::move(p)));
}

UnivariateExprPolynomial UnivariateSeries::pow(const UnivariateExprPolynomial &base, int exp, unsigned prec) {
    if (exp < 0)
            throw std::runtime_error("Not Implemented");
    if (exp == 0) {
        if (base == 0) {
            throw std::runtime_error("Error: 0**0 is undefined.");
        } else {
            return UnivariateExprPolynomial(1);
        }
    }
    UnivariateExprPolynomial x(base);
    UnivariateExprPolynomial y(1);
    while (exp > 1) {
        if (exp % 2 == 0) { 
            x = mul(x, x, prec);
            exp /= 2;
        } 
        else {
            y = mul(x, y, prec);
            x = mul(x, x, prec);
            exp = (exp - 1) / 2;
        }
    }
    return mul(x, y, prec);
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, unsigned deg) {
    return (s.get_univariate_poly()->get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

UnivariateExprPolynomial UnivariateSeries::diff(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    RCP<const Basic> p = s.get_univariate_poly()->diff(var.get_univariate_poly()->get_var());
    if (is_a<const UnivariatePolynomial>(*p))
        return UnivariateExprPolynomial(rcp_static_cast<const UnivariatePolynomial>(p));
    else
        throw std::runtime_error("Not a UnivariatePolynomial");
}

UnivariateExprPolynomial UnivariateSeries::integrate(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    map_int_Expr dict;
    for (auto &it : s.get_univariate_poly()->get_dict()) {
        if (it.first != -1) {
            dict.insert(std::pair<int, Expression>(it.first + 1, it.second / (it.first + 1)));
        } else {
            throw std::runtime_error("Not Implemented");
        }
    }
    return UnivariateExprPolynomial(univariate_polynomial(var.get_univariate_poly()->get_var(), (--dict.end())->first, std::move(dict))); 
}

UnivariateExprPolynomial UnivariateSeries::subs(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, const UnivariateExprPolynomial &r, unsigned prec) {
    throw std::runtime_error("Not Implemented");
}

Expression UnivariateSeries::sin(const Expression& c) {
    return SymEngine::sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression& c) {
    return cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression& c) {
    return tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression& c) {
    return asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression& c) {
    return acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression& c) {
    return atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression& c) {
    return sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression& c) {
    return cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression& c) {
    return tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression& c) {
    return asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression& c) {
    return atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression& c) {
    return exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression& c) {
    return log(c.get_basic());
}

} // SymEngine
