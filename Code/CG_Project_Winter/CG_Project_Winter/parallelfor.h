#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

// Taken from [https://stackoverflow.com/questions/36246300/parallel-loops-in-c]
// Credit goes to the user 'Arkan'

/// @param[in] nb_elements : size of your for loop
/// @param[in] functor(start, end) :
/// your function processing a sub chunk of the for loop.
/// "start" is the first index to process (included) until the index "end"
/// (excluded)
/// @code
///     for(int i = start; i < end; ++i)
///         computation(i);
/// @endcode
/// @param use_threads : enable / disable threads.
///
///
static void parallel_for(unsigned                                nb_elements,
                         std::function<void(int start, int end)> functor,
                         bool                                    use_threads = true)
{
    // -------
    unsigned nb_threads_hint = std::thread::hardware_concurrency();
    unsigned nb_threads      = nb_threads_hint == 0 ? 8 : (nb_threads_hint);
    // nb_threads -= 4;

    std::cout << "Thread hint is: " << nb_threads_hint << " and actual thread pool size is: " << nb_threads << std::endl;

    unsigned batch_size      = nb_elements / nb_threads;
    unsigned batch_remainder = nb_elements % nb_threads;

    std::vector<std::thread> my_threads(nb_threads);

    if (use_threads) {
        // Multithread execution.
        for (unsigned i = 0; i < nb_threads; ++i) {
            int start     = i * batch_size;
            my_threads[i] = std::thread(functor, start, start + batch_size);
        }
    } else {
        // Single thread execution (for easy debugging).
        for (unsigned i = 0; i < nb_threads; ++i) {
            int start = i * batch_size;
            functor(start, start + batch_size);
        }
    }

    // Deform the elements left.
    int start = nb_threads * batch_size;
    functor(start, start + batch_remainder);

    // Wait for the other thread to finish their task.
    if (use_threads)
        std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));
}