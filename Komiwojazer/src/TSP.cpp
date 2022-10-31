#include "TSP.hpp"

#include <algorithm>
#include <stack>
#include <optional>

std::ostream& operator<<(std::ostream& os, const CostMatrix& cm) {
    for (std::size_t r = 0; r < cm.size(); ++r) {
        for (std::size_t c = 0; c < cm.size(); ++c) {
            const auto& elem = cm[r][c];
            os << (is_inf(elem) ? "INF" : std::to_string(elem)) << " ";
        }
        os << "\n";
    }
    os << std::endl;

    return os;
}

/* PART 1 */

/**
 * Create path from unsorted path and last 2x2 cost matrix.
 * @return The vector of consecutive vertex.
 */
path_t StageState::get_path()
{
    // redukcja macierzy
    reduce_cost_matrix();

    // dodanie ostatnich dwóch vertexów do unsorted_path_
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        for (std::size_t j = 0; j < matrix_.size(); j++)
        {
            if (matrix_[i][j] == 0)
            {
                vertex_t v_n(i, j);
                append_to_path(v_n);
            }
        }
    }

    // szukanie końcowego cyklu
    std::size_t col = unsorted_path_[0].col;
    path_t sorted = {unsorted_path_[0].col + 1};
    std::size_t counter = 1;
    while (counter < unsorted_path_.size())
    {
        for (auto vertex : unsorted_path_)
        {
            if (vertex.row == col)
            {
                col = vertex.col;
                sorted.push_back(vertex.col + 1);
                counter++;
            }
        }
    }

    return sorted;
}

/**
 * Get minimum values from each row and returns them.
 * @return Vector of minimum values in row.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_rows() const
{
    std::vector<cost_t> min_values;
    for (auto vec : matrix_)
    {
        if (*std::min_element(vec.begin(), vec.end()) != INF)
        {
            min_values.push_back(*std::min_element(vec.begin(), vec.end()));
        }
        else
        {
            min_values.push_back(0);
        }
    }
    return min_values;
}

/**
 * Reduce rows so that in each row at least one zero value is present.
 * @return Sum of values reduced in rows.
 */
cost_t CostMatrix::reduce_rows()
{
    std::vector<cost_t> min_values = get_min_values_in_rows();
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        for (std::size_t j = 0; j < matrix_.size(); j++)
        {
            if (matrix_[i][j] != INF)
            {
                matrix_[i][j] -= min_values[i];
            }
        }
    }
    return std::accumulate(min_values.begin(), min_values.end(), 0);
}

/**
 * Get minimum values from each column and returns them.
 * @return Vector of minimum values in columns.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_cols() const
{
    std::vector<cost_t> min_values;
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        cost_t min = matrix_[0][i];
        for (std::size_t j = 0; j < matrix_.size(); j++)
        {
            if (min > matrix_[j][i])
            {
                min = matrix_[j][i];
            }
        }
        if (min != INF)
        {
            min_values.push_back(min);
        }
        else
        {
            min_values.push_back(0);
        }

    }
    return min_values;
}

/**
 * Reduces rows so that in each column at least one zero value is present.
 * @return Sum of values reduced in columns.
 */
cost_t CostMatrix::reduce_cols()
{
    std::vector<cost_t> min_values = get_min_values_in_cols();
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        for (std::size_t j = 0; j < matrix_.size(); j++)
        {
            if (matrix_[i][j] != INF)
            {
                matrix_[i][j] -= min_values[j];
            }
        }
    }
    return std::accumulate(min_values.begin(), min_values.end(), 0);
}

/**
 * Get the cost of not visiting the vertex_t (@see: get_new_vertex())
 * @param row
 * @param col
 * @return The sum of minimal values in row and col, excluding the intersection value.
 */
cost_t CostMatrix::get_vertex_cost(std::size_t row, std::size_t col) const
{
    std::vector<cost_t> min_rows;
    std::vector<cost_t> min_cols;
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        if (matrix_[i][col] != INF)
        {
            if (i != row)
            {
                min_rows.push_back(matrix_[i][col]);
            }
        }
    }
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        if (matrix_[row][i] != INF)
        {
            if (i != col)
            {
                min_cols.push_back(matrix_[row][i]);
            }
        }
    }

    cost_t s1 = *std::min_element(min_rows.begin(), min_rows.end());
    cost_t s2 = *std::min_element(min_cols.begin(), min_cols.end());

    return s1 + s2;
}

/* PART 2 */

/**
 * Choose next vertex to visit:
 * - Look for vertex_t (pair row and column) with value 0 in the current cost matrix.
 * - Get the vertex_t cost (calls get_vertex_cost()).
 * - Choose the vertex_t with maximum cost and returns it.
 * @param cm
 * @return The coordinates of the next vertex.
 */
NewVertex StageState::choose_new_vertex()
{
    std::vector<NewVertex> vec;
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        for (std::size_t j = 0; j < matrix_.size(); j++)
        {
            if (matrix_[i][j] == 0)
            {
                vertex_t ver(i, j);
                NewVertex n_ver(ver, matrix_.get_vertex_cost(i, j));
                vec.push_back(n_ver);
            }
        }
    }
    std::size_t iter = 0;
    cost_t max_cost = vec[0].cost;
    for (std::size_t i = 1; i < vec.size(); i++)
    {
        if (max_cost < vec[i].cost)
        {
            max_cost = vec[i].cost;
            iter = i;
        }
    }
    return vec[iter];
}

/**
 * Update the cost matrix with the new vertex.
 * @param new_vertex
 */
void StageState::update_cost_matrix(vertex_t new_vertex)
{
    matrix_[new_vertex.col][new_vertex.row] = INF;
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        matrix_[i][new_vertex.col] = INF;
    }
    for (std::size_t i = 0; i < matrix_.size(); i++)
    {
        matrix_[new_vertex.row][i] = INF;
    }
    // szukanie cykli
    for (auto start_vertex : unsorted_path_)
    {
        std::size_t start = start_vertex.row;
        std::size_t col = start_vertex.col;
        std::size_t i = 0;
        while (i < unsorted_path_.size())
        {
            if (unsorted_path_[i].row == col)
            {
                col = unsorted_path_[i].col;
                matrix_[unsorted_path_[i].col][start] = INF;
                i = 0;
            }
            else
            {
                i++;
            }
        }
    }
}

/**
 * Reduce the cost matrix.
 * @return The sum of reduced values.
 */
cost_t StageState::reduce_cost_matrix()
{
    return  matrix_.reduce_rows() + matrix_.reduce_cols();
}

/**
 * Given the optimal path, return the optimal cost.
 * @param optimal_path
 * @param m
 * @return Cost of the path.
 */
cost_t get_optimal_cost(const path_t& optimal_path, const cost_matrix_t& m) {
    cost_t cost = 0;

    for (std::size_t idx = 1; idx < optimal_path.size(); ++idx) {
        cost += m[optimal_path[idx - 1] - 1][optimal_path[idx] - 1];
    }

    // Add the cost of returning from the last city to the initial one.
    cost += m[optimal_path[optimal_path.size() - 1] - 1][optimal_path[0] - 1];

    return cost;
}

/**
 * Create the right branch matrix with the chosen vertex forbidden and the new lower bound.
 * @param m
 * @param v
 * @param lb
 * @return New branch.
 */
StageState create_right_branch_matrix(cost_matrix_t m, vertex_t v, cost_t lb) {
    CostMatrix cm(m);
    cm[v.row][v.col] = INF;
    return StageState(cm, {}, lb);
}

/**
 * Retain only optimal ones (from all possible ones).
 * @param solutions
 * @return Vector of optimal solutions.
 */
tsp_solutions_t filter_solutions(tsp_solutions_t solutions) {
    cost_t optimal_cost = INF;
    for (const auto& s : solutions) {
        optimal_cost = (s.lower_bound < optimal_cost) ? s.lower_bound : optimal_cost;
    }

    tsp_solutions_t optimal_solutions;
    std::copy_if(solutions.begin(), solutions.end(),
                 std::back_inserter(optimal_solutions),
                 [&optimal_cost](const tsp_solution_t& s) { return s.lower_bound == optimal_cost; }
    );

    return optimal_solutions;
}

/**
 * Solve the TSP.
 * @param cm The cost matrix.
 * @return A list of optimal solutions.
 */
tsp_solutions_t solve_tsp(const cost_matrix_t& cm) {

    StageState left_branch(cm);

    // The branch & bound tree.
    std::stack<StageState> tree_lifo;

    // The number of levels determines the number of steps before obtaining
    // a 2x2 matrix.
    std::size_t n_levels = cm.size() - 2;

    tree_lifo.push(left_branch);   // Use the first cost matrix as the root.

    cost_t best_lb = INF;
    tsp_solutions_t solutions;

    while (!tree_lifo.empty()) {

        left_branch = tree_lifo.top();
        tree_lifo.pop();

        while (left_branch.get_level() != n_levels && left_branch.get_lower_bound() <= best_lb) {
            // Repeat until a 2x2 matrix is obtained or the lower bound is too high...

            if (left_branch.get_level() == 0) {
                left_branch.reset_lower_bound();
            }

            // 1. Reduce the matrix in rows and columns.
            cost_t new_cost = left_branch.reduce_cost_matrix(); // @TODO (KROK 1)


            // 2. Update the lower bound and check the break condition.
            left_branch.update_lower_bound(new_cost);
            if (left_branch.get_lower_bound() > best_lb) {
                break;
            }

            // 3. Get new vertex and the cost of not choosing it.
            NewVertex new_vertex = NewVertex(left_branch.choose_new_vertex()); // @TODO (KROK 2)

            // 4. @TODO Update the path - use append_to_path method.
            left_branch.append_to_path(new_vertex.coordinates);

            // 5. @TODO (KROK 3) Update the cost matrix of the left branch.
            left_branch.update_cost_matrix(new_vertex.coordinates);

            // 6. Update the right branch and push it to the LIFO.
            cost_t new_lower_bound = left_branch.get_lower_bound() + new_vertex.cost;
            tree_lifo.push(create_right_branch_matrix(cm, new_vertex.coordinates,
                                                      new_lower_bound));
        }

        if (left_branch.get_lower_bound() <= best_lb) {
            // If the new solution is at least as good as the previous one,
            // save its lower bound and its path.
            best_lb = left_branch.get_lower_bound();
            path_t new_path = left_branch.get_path();
            solutions.push_back({get_optimal_cost(new_path, cm), new_path});
        }
    }

    return filter_solutions(solutions); // Filter solutions to find only optimal ones.
}
