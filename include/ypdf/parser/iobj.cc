// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/iobj.hh>
#include <ypdf/parser/iobj.hh>
#include <ypdf/parser/obj.hh>
#include <ypdf/parser/skip.hh>
#include <ypdf/parser/stream.hh>

namespace ypdf::parser {

template< typename Iterator >
bool iobj(Iterator first, Iterator &iter, Iterator last, ast::iobj_t &attr)
{
    Iterator where = iter;

    int num;
    if (int_(first, iter, last, num)) {
        skipws(first, iter, last);

        int gen = 0;
        if (int_(first, iter, last, gen)) {
            skipws(first, iter, last);

            if (lit(first, iter, last, "obj")) {
                skipws(first, iter, last);

                ast::array_t subobjs;

                skipws(first, iter, last);
                comment(first, iter, last);

                for (; iter != last;) {
                    if (lookahead(iter, last, "endobj")) {
                        lit(first, iter, last, "endobj");

                        attr = ast::iobj_t{
                            ast::basic_xref_t{
                                { num, gen },
                                std::distance(first, where)
                            }, ast::obj_t(std::move(subobjs))
                        };

                        return true;
                    }
                    else {
                        ast::obj_t subobj;

                        if (!obj(first, iter, last, subobj))
                            return false;

                        subobjs.emplace_back(std::move(subobj));
                    }

                    skipws(first, iter, last);
                    comment(first, iter, last);
                }
            }
        }
    }

    return false;
}

} // ypdf::parser

