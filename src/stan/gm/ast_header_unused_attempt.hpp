#ifndef __STAN__GM__AST_HPP__
#define __STAN__GM__AST_HPP__

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <cstddef>
#include <limits>
#include <climits>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace stan {

  namespace gm {

    /** Placeholder struct for boost::variant default ctors 
     */
    struct nil { };

    // components of abstract syntax tree 
    struct assignment;
    struct binary_op;
    struct distribution;
    struct double_var_decl;
    struct double_literal;
    struct expression;
    struct for_statement;
    struct fun;
    struct identifier;
    struct index_op;
    struct int_literal;
    struct inv_var_decl;
    struct matrix_var_decl;
    struct no_op_statement;
    struct pos_ordered_var_decl;
    struct program;
    struct range;
    struct row_vector_var_decl;
    struct sample;
    struct simplex_var_decl;
    struct statement;
    struct statements;
    struct unary_op;
    struct variable;
    struct variable_dims;
    struct var_decl;
    struct var_type;
    struct vector_var_decl;

    // enum base_expr_type {
    //   INT_T,
    //   DOUBLE_T,
    //   VECTOR_T, // includes: SIMPLEX_T, POS_ORDERED_T
    //   ROW_VECTOR_T,
    //   MATRIX_T,
    //   ILL_FORMED_T // includes: CORR_MATRIX_T, COV_MATRIX_T
    // };

    // std::ostream& operator<<(std::ostream& o, base_expr_type type) {
    //   switch (type) {
    //   case INT_T :
    //     o << "int";
    //     break;
    //   case DOUBLE_T :
    //     o << "double";
    //     break;
    //   case VECTOR_T :
    //     o << "vector";
    //     break;
    //   case ROW_VECTOR_T :
    //     o << "row vector";
    //     break;
    //   case MATRIX_T :
    //     o << "matrix";
    //     break;
    //   case ILL_FORMED_T :
    //     o << "ill formed";
    //     break;
    //   default:
    //     o << "UNKNOWN";
    //   }
    //   return o;
    // }

    // forward declarable enum hack
    typedef int base_expr_type;
    const int INT_T = 1;
    const int DOUBLE_T = 2;
    const int VECTOR_T = 3;
    const int ROW_VECTOR_T = 4;
    const int MATRIX_T = 5;
    const int ILL_FORMED_T = 6;

    // // can't forward-declare an enum
    // enum base_expr_type {
    //   INT_T,
    //   DOUBLE_T,
    //   VECTOR_T, // includes: SIMPLEX_T, POS_ORDERED_T
    //   ROW_VECTOR_T,
    //   MATRIX_T,
    //   ILL_FORMED_T // includes: CORR_MATRIX_T, COV_MATRIX_T
    // };

    std::ostream& write_base_expr_type(std::ostream& o, base_expr_type type);

    struct expr_type {
      base_expr_type base_type_;
      size_t num_dims_;
      expr_type();
      expr_type(const base_expr_type base_type); 
      expr_type(const base_expr_type base_type,
                size_t num_dims); 
      bool operator==(const expr_type& et) const;
      bool operator!=(const expr_type& et) const;
      bool is_primitive() const;
      bool is_primitive_int() const;
      bool is_primitive_double() const;
      bool is_ill_formed() const;
      base_expr_type type() const;
      size_t num_dims() const;
    };

    std::ostream& operator<<(std::ostream& o, const expr_type& et);

    expr_type promote_primitive(const expr_type& et);

    expr_type promote_primitive(const expr_type& et1,
                                const expr_type& et2);

    typedef std::pair<expr_type, std::vector<expr_type> > function_signature_t;

    class function_signatures {
    public:
      static function_signatures& instance();
      void add(const std::string& name,
               const expr_type& result_type,
               const std::vector<expr_type>& arg_types);
      void add(const std::string& name,
               const expr_type& result_type);
      void add(const std::string& name,
               const expr_type& result_type,
               const expr_type& arg_type);
      void add(const std::string& name,
               const expr_type& result_type,
               const expr_type& arg_type1,
               const expr_type& arg_type2);
      void add(const std::string& name,
               const expr_type& result_type,
               const expr_type& arg_type1,
               const expr_type& arg_type2,
               const expr_type& arg_type3);
      void add(const std::string& name,
               const expr_type& result_type,
               const expr_type& arg_type1,
               const expr_type& arg_type2,
               const expr_type& arg_type3,
               const expr_type& arg_type4);
      void add_nullary(const::std::string& name);
      void add_unary(const::std::string& name);
      void add_binary(const::std::string& name);
      void add_ternary(const::std::string& name);
      void add_quaternary(const::std::string& name);
      int num_promotions(const std::vector<expr_type>& call_args,
                         const std::vector<expr_type>& sig_args);
      expr_type get_result_type(const std::string& name,
                                const std::vector<expr_type>& args);
    private:
      function_signatures(); 
      function_signatures(const function_signatures& fs);
      std::map<std::string, std::vector<function_signature_t> > sigs_map_;
      static function_signatures* sigs_;  // init below outside of class
    };

    struct statements {
      std::vector<var_decl> local_decl_;
      std::vector<statement> statements_;
      statements();
      statements(const std::vector<var_decl>& local_decl,
                 const std::vector<statement>& stmts);
    };


    struct distribution {
      std::string family_;
      std::vector<expression> args_;
    };

    struct expression_type_vis : public boost::static_visitor<expr_type> {
      expr_type operator()(const nil& e) const;
      template <typename T> expr_type operator()(const T& e) const;
    };

    struct expression;

    struct expression {
      typedef boost::variant<boost::recursive_wrapper<nil>, 
                             boost::recursive_wrapper<int_literal>,
                             boost::recursive_wrapper<double_literal>,
                             boost::recursive_wrapper<variable>,
                             boost::recursive_wrapper<fun>,
                             boost::recursive_wrapper<index_op>,
                             boost::recursive_wrapper<binary_op>,
                             boost::recursive_wrapper<unary_op> > 
      expression_t;

      expression();
      expression(const expression& e);
      template <typename Expr> expression(const Expr& expr);

      expr_type expression_type() const; 

      expression& operator+=(const expression& rhs);
      expression& operator-=(const expression& rhs);
      expression& operator*=(const expression& rhs);
      expression& operator/=(const expression& rhs);

      expression_t expr_;
    };

    struct is_nil_op : public boost::static_visitor<bool> {
      bool operator()(nil const& x) const;
      template <typename T>
      bool operator()(const T& x) const;
    };

    bool is_nil(const expression& e);

    struct variable_dims {
      std::string name_;
      std::vector<expression> dims_;
      variable_dims();
      variable_dims(std::string const& name,
                    std::vector<expression> const& dims); 
    };


    struct int_literal {
      int val_;
      expr_type type_;
      int_literal();
      int_literal(int val);
      int_literal(const int_literal& il);
      int_literal& operator=(const int_literal& il);
    };


    struct double_literal {
      double val_;
      expr_type type_;
      double_literal();
      double_literal(double val);
      double_literal& operator=(const double_literal& dl);
    };

    struct variable {
      std::string name_;
      expr_type type_;
      variable();
      variable(std::string name);
      void set_type(const base_expr_type& base_type, 
                    size_t num_dims);
    };

    struct fun {
      std::string name_;
      std::vector<expression> args_;
      expr_type type_;
      fun();
      fun(std::string const& name,
          std::vector<expression> const& args); 
      void infer_type();  // FIXME: is this used anywhere?
    };

    size_t total_dims(const std::vector<std::vector<expression> >& dimss);

    expr_type infer_type_indexing(const base_expr_type& expr_base_type,
                                  size_t num_expr_dims,
                                  size_t num_index_dims);

    expr_type infer_type_indexing(const expression& expr,
                                  size_t num_index_dims);


    struct index_op {
      expression expr_;
      std::vector<std::vector<expression> > dimss_;
      expr_type type_;
      index_op();
      // vec of vec for e.g., e[1,2][3][4,5,6]
      index_op(const expression& expr,
               const std::vector<std::vector<expression> >& dimss);
      void infer_type();
    };


    struct binary_op {
      char op;
      expression left;
      expression right;
      expr_type type_;
      binary_op();
      binary_op(const expression& left,
                char op,
                const expression& right);
    };

   struct unary_op {
      char op;
      expression subject;
      expr_type type_;
      unary_op(char op,
               expression const& subject);
    };

    struct range {
      expression low_;
      expression high_;
      range();
      range(expression const& low,
            expression const& high);
      bool has_low() const;
      bool has_high() const;
    };

    typedef int var_origin;
    const int data_origin = 1;
    const int transformed_data_origin = 2;
    const int parameter_origin = 3;
    const int transformed_parameter_origin = 4;
    const int derived_origin = 5;
    const int local_origin = 6;

    // enum var_origin {
    //    data_origin,
    //    transformed_data_origin,
    //    parameter_origin,
    //    transformed_parameter_origin,
    //    derived_origin,
    //    local_origin
    //  };

    std::ostream& operator<<(std::ostream& o, const var_origin& vo) {
      if (vo == data_origin)
        o << "data";
      else if (vo == transformed_data_origin)
        o << "transformed data";
      else if (vo == parameter_origin) 
        o << "parameter";
      else if (vo == transformed_parameter_origin)
        o << "transformed parameter";
      else if (vo == derived_origin)
        o << "derived";
      else if (vo == local_origin)
        o << "local";
      else 
        o << "UNKNOWN ORIGIN";
      return o;
    }    



    void print_var_origin(std::ostream& o, const var_origin& vo);

    // FIXME: replace < with print_var_ori
    // std::ostream& operator<<(std::ostream& o, const var_origin& vo);


    struct base_var_decl {
      std::string name_;
      std::vector<expression> dims_;
      base_expr_type base_type_;
      base_var_decl();
      base_var_decl(const base_expr_type& base_type); 
      base_var_decl(const std::string& name,
                    const std::vector<expression>& dims,
                    const base_expr_type& base_type);
    };

    struct variable_map {
      typedef std::pair<base_var_decl,var_origin> range_t;
      std::map<std::string, range_t> map_;
      bool exists(const std::string& name) const;
      base_var_decl get(const std::string& name) const;
      base_expr_type get_base_type(const std::string& name) const;
      size_t get_num_dims(const std::string& name) const;
      var_origin get_origin(const std::string& name) const;
      void add(const std::string& name,
               const base_var_decl& base_decl,
               const var_origin& vo);
      void remove(const std::string& name);
    };

    struct int_var_decl : public base_var_decl {
      range range_;
      int_var_decl(); 
      int_var_decl(range const& range,
                   std::string const& name,
                   std::vector<expression> const& dims); 
    };


    struct double_var_decl : public base_var_decl {
      range range_;
      double_var_decl();
      double_var_decl(range const& range,
                      std::string const& name,
                      std::vector<expression> const& dims);
    };

    struct simplex_var_decl : public base_var_decl {
      expression K_;
      simplex_var_decl();
      simplex_var_decl(expression const& K,
                       std::string const& name,
                       std::vector<expression> const& dims);
    };

    struct pos_ordered_var_decl : public base_var_decl {
      std::string name_;
      expression K_;
      std::vector<expression> dims_;
      pos_ordered_var_decl();
      pos_ordered_var_decl(expression const& K,
                           std::string const& name,
                           std::vector<expression> const& dims);
    };

    struct vector_var_decl : public base_var_decl {
      expression M_;
      vector_var_decl();
      vector_var_decl(expression const& M,
                      std::string const& name,
                      std::vector<expression> const& dims);
    };

    struct row_vector_var_decl : public base_var_decl {
      expression N_;
      row_vector_var_decl();
      row_vector_var_decl(expression const& N,
                          std::string const& name,
                          std::vector<expression> const& dims);
    };

    struct matrix_var_decl : public base_var_decl {
      expression M_;
      expression N_;
      matrix_var_decl();
      matrix_var_decl(expression const& M,
                      expression const& N,
                      std::string const& name,
                      std::vector<expression> const& dims);
    };

   

   

    struct cov_matrix_var_decl : public base_var_decl {
      std::string name_;
      expression K_;
      std::vector<expression> dims_;
      cov_matrix_var_decl();
      cov_matrix_var_decl(expression const& K,
                           std::string const& name,
                          std::vector<expression> const& dims);
    };



    struct corr_matrix_var_decl : public base_var_decl {
      std::string name_;
      expression K_;
      std::vector<expression> dims_;
      corr_matrix_var_decl();
      corr_matrix_var_decl(expression const& K,
                           std::string const& name,
                           std::vector<expression> const& dims);
    };



    struct name_vis : public boost::static_visitor<std::string> {
      name_vis();
      std::string operator()(const nil& x) const;
      std::string operator()(const int_var_decl& x) const;
      std::string operator()(const double_var_decl& x) const;
      std::string operator()(const vector_var_decl& x) const;
      std::string operator()(const row_vector_var_decl& x) const;
      std::string operator()(const matrix_var_decl& x) const;
      std::string operator()(const simplex_var_decl& x) const;
      std::string operator()(const pos_ordered_var_decl& x) const;
      std::string operator()(const cov_matrix_var_decl& x) const;
      std::string operator()(const corr_matrix_var_decl& x) const;
    };




    struct var_decl {
      typedef boost::variant<boost::recursive_wrapper<nil>,
                             boost::recursive_wrapper<int_var_decl>,
                             boost::recursive_wrapper<double_var_decl>,
                             boost::recursive_wrapper<vector_var_decl>,
                             boost::recursive_wrapper<row_vector_var_decl>,
                             boost::recursive_wrapper<matrix_var_decl>,
                             boost::recursive_wrapper<simplex_var_decl>,
                             boost::recursive_wrapper<pos_ordered_var_decl>,
                             boost::recursive_wrapper<cov_matrix_var_decl>,
                             boost::recursive_wrapper<corr_matrix_var_decl> >
    type;

    type decl_;

    var_decl();

    template <typename Decl>
    var_decl(Decl const& decl);

    std::string name() const;
  };

  struct statement {
    typedef boost::variant<boost::recursive_wrapper<nil>,
                           boost::recursive_wrapper<assignment>,
                           boost::recursive_wrapper<sample>,
                           boost::recursive_wrapper<statements>,
                           boost::recursive_wrapper<for_statement>,
                           boost::recursive_wrapper<no_op_statement> >
    type;
    
    type statement_;

    statement();

    statement(const nil& nil);

    template <typename Statement>
    statement(const Statement& statement);
  };


  struct for_statement {
    std::string variable_;
    range range_;
    statement statement_;
    for_statement();
    for_statement(std::string& variable,
                  range& range,
                  statement& stmt);
  };

  struct no_op_statement {
      // no op, no data
  };


   
    struct program {
      std::vector<var_decl> data_decl_;
      std::pair<std::vector<var_decl>,std::vector<statement> > 
      derived_data_decl_;
      std::vector<var_decl> parameter_decl_;
      std::pair<std::vector<var_decl>,std::vector<statement> > 
      derived_decl_;
      statement statement_;
      std::pair<std::vector<var_decl>,std::vector<statement> > generated_decl_;
      program();
      program(const std::vector<var_decl>& data_decl,
              const std::pair<std::vector<var_decl>,
                              std::vector<statement> >& derived_data_decl,
              const std::vector<var_decl>& parameter_decl,
              const std::pair<std::vector<var_decl>,
                              std::vector<statement> >& derived_decl,
              const statement& st,
              const std::pair<std::vector<var_decl>,
              std::vector<statement> >& generated_decl);


    };

  struct sample {
    expression expr_;
    distribution dist_;
    range truncation_;
    sample();
    sample(expression& e,
           distribution& dist);
    bool is_ill_formed() const;
  };

  struct assignment {
    variable_dims var_dims_; // lhs_var[dim0,...,dimN-1] 
    expression expr_;        // = rhs
    base_var_decl var_type_; // type of lhs_var
    assignment();
    assignment(variable_dims& var_dims,
               expression& expr);
  };
  
  // from generator.hpp
  void generate_expression(const expression& e, std::ostream& o);


  }
}
 
#endif