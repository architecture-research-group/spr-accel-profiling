/*******************************************************************************

 * Copyright (C) 2022 Intel Corporation

 *

 * SPDX-License-Identifier: MIT

 ******************************************************************************/


//* [QPL_LOW_LEVEL_COMPRESSION_EXAMPLE] */


#include <iostream>

#include <vector>

#include <memory>


#include "qpl/qpl.h"

constexpr const uint32_t source_size = 1000;


auto main(int argc, char** argv) -> int {

    std::cout << "Intel(R) Query Processing Library version is " << qpl_get_library_version() << ".\n";


    // Default to Software Path

    qpl_path_t execution_path = qpl_path_hardware;


    // Source and output containers

    std::vector<uint8_t> source(source_size, 5);

    std::vector<uint8_t> destination(source_size / 2, 4);


    std::unique_ptr<uint8_t[]> job_buffer;

    qpl_status                 status;

    uint32_t                   size = 0;


    // Job initialization

    status = qpl_get_job_size(execution_path, &size);

    if (status != QPL_STS_OK) {

        std::cout << "An error " << status << " acquired during job size getting.\n";

        return 1;

    }


    job_buffer = std::make_unique<uint8_t[]>(size);

    qpl_job *job = reinterpret_cast<qpl_job *>(job_buffer.get());


    status = qpl_init_job(execution_path, job);

    if (status != QPL_STS_OK) {

        std::cout << "An error " << status << " acquired during job initializing.\n";

        return 1;

    }


    // Performing a compression operation

    job->op            = qpl_op_compress;

    job->level         = qpl_default_level;

    job->next_in_ptr   = source.data();

    job->next_out_ptr  = destination.data();

    job->available_in  = source_size;

    job->available_out = static_cast<uint32_t>(destination.size());

    job->flags         = QPL_FLAG_FIRST | QPL_FLAG_LAST | QPL_FLAG_DYNAMIC_HUFFMAN | QPL_FLAG_OMIT_VERIFY;


    // Compression

    status = qpl_execute_job(job);

    if (status != QPL_STS_OK) {

        std::cout << "An error " << status << " acquired during compression.\n";

        return 1;

    }


    // Freeing resources

    status = qpl_fini_job(job);

    if (status != QPL_STS_OK) {

        std::cout << "An error " << status << " acquired during job finalization.\n";

        return 1;

    }


    return 0;

}


//* [QPL_LOW_LEVEL_COMPRESSION_EXAMPLE] */