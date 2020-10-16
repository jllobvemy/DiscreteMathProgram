#include <iostream>
#include <stack>
#include <vector>
#include <set>
#include <map>
using namespace std;

enum operate {
	major_disjunctive_form, major_conjunctive_form  // 主析取范式 主合取范式
};
// 去除低位的0, maxsize：二进制的最大位数

wstring inffix_to_suffix(const wstring& expression)
{
	stack<wchar_t> s;
	wstring ret;
	for (const auto& i : expression)
	{
		switch (i)
		{

		case L'(':
			s.push (i);
			break;
		case L')':
			while (!s.empty())
			{
				if (s.top() == L'(')
				{
					s.pop ();
					break;
				}
				ret += s.top ();
				s.pop ();
			}
			break;
		case L'┐':
			s.push (i);
			break;
		case L'∧':
		case L'∨':
			while (!s.empty() && s.top() != L'(' && s.top() != L'→' && s.top() != L'⇄')
			{
				ret += s.top ();
				s.pop ();
			}
			s.push (i);
			break;
		case L'→':
		case L'⇄':
			while (!s.empty() && s.top() != L'(')
			{
				ret += s.top ();
				s.pop ();
			}
			s.push (i);
			break;
		default: 
			ret += i;
			break;
		}
	}
	while (!s.empty())
	{
		ret += s.top ();
		s.pop ();
	}
	return ret;
}

unsigned int delete_tail_zero(unsigned int a)
{
	return a > 0 ? 1 : 0;
}

unsigned int calculate(unsigned int a, unsigned int b, wchar_t operate)
{
	a = delete_tail_zero (a);
	b = delete_tail_zero (b);
	switch (operate)  
	{
	case L'∨':
		return a | b;

	case L'∧':
		return a & b;

	case L'⇄':
		return (a == b);

	case L'→':
		return (!a) | b;

	default:
		throw exception ("表达式错误");
	//	exit(1);
	}
}

short calculate_not(unsigned int a)
{
	a = delete_tail_zero (a);
	return !a;
}

short calculate(const wstring& s) // 后缀二进制表达式运算
{
	stack<unsigned int> elem;
	unsigned int top1;
	unsigned int top2;
	for (auto c : s)
	{
		switch (c)
		{
		case L'1':
			elem.push(1);
			break;
		case L'0':
			elem.push(0);
			break;
		case L'∨':
		case L'∧':
		case L'⇄':
		case L'→':
			if (elem.empty())
				throw exception("表达式错误");
			top1 = elem.top();
			elem.pop();
			if (elem.empty())
				throw exception("表达式错误");
			top2 = elem.top();
			elem.pop();
			elem.push(calculate(top2, top1, c));  // 出栈顺序相反 只会影响单条件运算符
			break;
		case L'┐':		
			if (elem.empty())
				throw exception("表达式错误");
			top1 = elem.top();
			elem.pop();
			elem.push(calculate_not(top1));
			break;
		default:
			throw exception("表达式错误");
		}
		
	}
	if (elem.empty())
		throw exception("表达式错误");
	if (elem.size() > 1)
		throw exception("表达式错误");
	return elem.top ();
}
vector<int> get_bins (int size)
{
	vector<int> a;
	a.push_back (1);
	for (int i = 1; i < size; i++)
	{
		a.push_back (a.at (i - 1) << 1);
	}
	return a;
}
// 将字符串中的字母用二进制数字替换，便于直接计算结果, s: 含原子命题的表达式， a: 二进制表达式，character_name：原子命题数量
// 对于重复的字母，值始终是相同的
wstring convert_char_to_bin_digit(wstring s, unsigned int a, unsigned int character_num)
{
	int k = character_num;
	auto b = get_bins (character_num);
	set<wchar_t> elem;
	map<wchar_t, wchar_t> m;
	for (auto& c: s)
	{
		if (c <= L'Z' && c >= L'A' && k >= 0)
		{
			if (elem.count(c))
			{
				c = m.at(c);
			}
			else
			{
				auto d = (delete_tail_zero (a & b[--k])) ? L'1' : L'0';
				elem.insert (c);
				m.insert(pair(c, d));
				// 取每一位的值
				c = d;  // 将字母转为二进制数字
			}
		}
	}
	return s;
}
wstring get_minor_item(unsigned int a, const set<wchar_t>& elem) // 计算小项 主析取
{
	int size = elem.size ();
	auto bins = get_bins (elem.size ());
	wstring ans;
	for (const auto& i : elem)
	{
		if (!delete_tail_zero(a & bins[--size]))
		{
			ans += L'┐';
		}
		ans += i;
		ans += L'∧';
	}
	ans.pop_back ();
	return ans;
}
wstring get_big_item(unsigned int a, const set<wchar_t>& elem) // 计算大项 主合取
{
	int size = elem.size ();
	auto bins = get_bins (elem.size ());
	wstring ans;
	for (const auto& i : elem)
	{
		if (delete_tail_zero (a & bins[--size]))
		{
			ans += L'┐';
		}
		ans += i;
		ans += L'∨';
	}
	ans.pop_back ();
	return ans;
}

wstring get_answer(const wstring& expression, const operate o) // 计算结果
{
	set<wchar_t> elem;
	wstring ret;
	int maxsize = 1, character_num = 0;
	for (const auto& i : expression)
	{
		if (i >= L'A' && i <= L'Z')
		{
			character_num++;
			elem.insert (i);
		}
	}
	
	maxsize <<= elem.size ();
	if (o == major_disjunctive_form) // 求主析取范式
	{
		for (int i = 0; i < maxsize; i++)
		{
			wstring s = convert_char_to_bin_digit (expression, i, elem.size());
			int ans = calculate (s);
			// 逐个获取小项
			if (ans)
			{
				ret += L'(';
				ret += get_minor_item (i, elem);
				ret += L')';
				ret += L'∨';	
			}
		}
		if (ret.empty ())
			return L"无主析取范式";
		ret.pop_back ();
		return ret;
	}
	else // 求主合取范式
	{
		for (int i = 0; i < maxsize; i++)
		{
			wstring s = convert_char_to_bin_digit (expression, i, elem.size ());
			int ans = calculate (s);
			if (!ans)
			{
				ret += L'(';
				ret += get_big_item (i, elem);
				ret += L')';
				ret += L'∧';
			}
		}
		if (ret.empty ())
			return L"无主合取范式";
		ret.pop_back ();
		return ret;
	}
}
// dll入口
extern "C" __declspec(dllexport)
const wchar_t* __cdecl CalculateForm(wchar_t* expression,  int maxsize, int operate) // 0: 主析取范式 1: 主合取范式
{
	setlocale (LC_ALL, "chs");
	wchar_t* ret = new wchar_t[maxsize];
	if (wcslen(expression) == 0)
	{
		wcscpy_s(ret, maxsize, L"");
		return ret;
	}

	try
	{
		wstring s;
		if (operate == 0)
			s = get_answer(wstring(inffix_to_suffix(expression)), major_disjunctive_form);
		else
			s = get_answer(wstring(inffix_to_suffix(expression)), major_conjunctive_form);
		wcscpy_s (ret, maxsize, s.c_str());
		return ret;

	}
	catch (exception e)
	{
		swprintf(ret, maxsize, L"%hs", e.what());
		return ret;
	}
	
}

extern "C" __declspec(dllexport)
void __cdecl FreeMemory(void* expression)
{
	delete[] expression;
}
