#include"exercise15_35_TextQuery.h"

class Query_base {
	friend class Query;
protected:
	using line_no = std::vector<std::string>::size_type;
	virtual ~Query_base() = default;
private:
	virtual QueryResult eval(const TextQuery&) const = 0;
	virtual std::string rep() const = 0;
};

class Query {
	friend Query operator~(const Query&);
	friend Query operator|(const Query&, const Query&);
	friend Query operator&(const Query&, const Query&);

public:
	Query(const std::string&);
	QueryResult eval(const TextQuery& t) const { return q->eval(t); }
	std::string rep() const { return q->rep(); }

private:
	Query(std::shared_ptr<Query_base> query) :q(query) {}
	std::shared_ptr<Query_base> q;
};

std::ostream& operator<<(std::ostream& os, const Query& query) {
	return os << query.rep();
}

class WordQuery :public Query_base {
	friend class Query;
	WordQuery(const std::string& s) :query_word(s) {}

	QueryResult eval(const TextQuery& t) const { return t.query(query_word); }
	std::string rep() const { return query_word; }
	std::string query_word;
};
//now we can implement Query constructor
inline Query::Query(const std::string& s) :q(new WordQuery(s)) {}

class NotQuery :public Query_base {
	friend Query operator~(const Query&);
	NotQuery(const Query& q) :query(q) {}

	std::string rep() const { return "~(" + query.rep() + ")"; }
	QueryResult eval(const TextQuery&) const;
	Query query;
};

inline Query operator~(const Query& operand)
{
	return std::shared_ptr<Query_base>(new NotQuery(operand));
}

class BinaryQuery :public Query_base {
protected:
	BinaryQuery(const Query& l, const Query& r, std::string s) :
		lhs(l), rhs(r), opSym(s) {}
	std::string rep() const { return"(" + lhs.rep() + " " + opSym + " " + rhs.rep() + ")"; }
	Query lhs, rhs;
	std::string opSym;
};

class AndQuery : public BinaryQuery {
	friend Query operator &(const Query&, const Query&);
	AndQuery(const Query& left, const Query& right) :
		BinaryQuery(left, right, "&") {}
	QueryResult eval(const TextQuery&) const;

};
inline Query operator&(const Query& lhs, const Query& rhs) {
	return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}

class OrQuery : public BinaryQuery {
	friend Query operator|(const Query&, const Query&);
	OrQuery(const Query& left, const Query& right) :
		BinaryQuery(left, right, "|") {}
	QueryResult eval(const TextQuery&) const;
};

inline Query operator|(const Query& lhs, const Query& rhs) {
	return std::shared_ptr<Query_base>(new OrQuery(lhs, rhs));
}


QueryResult OrQuery::eval(const TextQuery& text) const
{
	auto right = rhs.eval(text), left = lhs.eval(text);
	auto ret_lines = std::make_shared<std::set<line_no>>(left.begin(), right.end());

	ret_lines->insert(right.begin(), right.end());

	//���ɽ��ͼ
	auto res = right.get_result();
	res.insert(left.get_result().begin(), left.get_result().end());

	return QueryResult(rep(), ret_lines->size(), res, left.get_file());
}

QueryResult AndQuery::eval(const TextQuery& text) const
{
	auto left = lhs.eval(text), right = rhs.eval(text);
	auto ret_lines = std::make_shared<set<line_no>>();
	std::set_intersection(left.begin(), left.end(), right.begin(), right.end(),
		inserter(*ret_lines, ret_lines->begin()));
	map<size_t, std::string> res;

	for (auto line : *ret_lines) {
		std::string s = left.get_file()->at(line);
		res.insert({ line, s });
	}

	return QueryResult(rep(), ret_lines->size(), res, left.get_file());

}

QueryResult NotQuery::eval(const TextQuery& text) const
{
	auto result = query.eval(text);

	auto ret_lines = make_shared<set<line_no>>();
	auto beg = result.begin(), end = result.end();
	auto sz = result.get_file()->size();

	for (size_t n = 0; n != sz; ++n) {
		if (beg == end || *beg != n) {
			ret_lines->insert(n);
		}
		else if (beg != end) {
			++beg;
		}
	}

	map<size_t, std::string> res;

	for (auto line : *ret_lines) {
		std::string s = result.get_file()->at(line);
		res.insert({ line, s });
	}

	return QueryResult(rep(), ret_lines->size(), res, result.get_file());
}