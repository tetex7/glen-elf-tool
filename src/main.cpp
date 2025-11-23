/*
 * Copyright (C) 2025  Tetex7
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

//
// Created by tete on 11/18/2025.
//

#include <boost/program_options.hpp>
#include <glen_elf_tool_config.h>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include "kelf.h"
#include <filesystem>
#include <print>
#include <cstring>

using std::literals::operator ""s;

namespace po = boost::program_options;

template<typename... vT>
[[__gnu__::__always_inline__]] __inline void cout_print(const vT&... vals)
{
    (std::cout << ... << vals);
}

#define REP_BUG_TEXT "Copyright (C) 2025  Tetex7.\nFor Docs and bug reporting\nplease see: <https://github.com/tetex7/glen_elf_tool>."
static void help(po::options_description& desc)
{
    cout_print("Usage: glen_elf_tool [options] --elf [PATH_TO_ELF]\n");
    cout_print(desc);
    cout_print("\n\n");
    cout_print("K-types:\n");
    cout_print("    binary (The main kernel binary)\n");
    cout_print("    driver (A kernel driver)\n");
    cout_print("    module (A generic kernel module)\n");
    cout_print("\n");
    cout_print("loader-features:\n");
    cout_print("    K_LOADER_MK_VMS (informs the loader to prepare provisional Virtual memory space)\n");
    cout_print("    K_LOADER_NO_LOADER_FEATURES (Request no features)\n");
    cout_print("\n");
    cout_print("version:   ", GLEN_ELF_TOOL_CONFIG_STR_VERSION, '\n');
    cout_print("Built for: ", GLEN_ELF_TOOL_SYSTEM_BUILT_FOR, '\n');
    cout_print("Commit:    ", GLEN_ELF_TOOL_GIT_COMMIT_HASH, '\n');
    cout_print("Branch:    ", GLEN_ELF_TOOL_GIT_BRANCH_NAME, '\n');
    cout_print("Compiler:  ", GLEN_ELF_TOOL_C_COMPILER, '\n');
    cout_print(REP_BUG_TEXT, '\n');
}

Elf64_Byte to_load_feature(const std::string& feature_name)
{
    if (feature_name == "K_LOADER_MK_VMS")
        return K_LOADER_MK_VMS;
    return K_LOADER_NO_LOADER_FEATURES;

}

std::string to_ext_str(const std::string& ext_name)
{
    if (ext_name == "binary")
            return K_KEN_BIN_EXT_STR;
    else if (ext_name == "driver")
        return K_KEN_DRIVER_EXT_STR;
    else if (ext_name == "module")
            return K_KEN_MODULE_EXT_STR;
    throw std::runtime_error("Invalid ext type");
}

std::string str_k_type(const Elf64_Char k_type[])
{
    if (!memcmp(k_type, K_KEN_BIN_EXT_STR, EI_K_EXT_STR))
        return "binary";
    else if (!memcmp(k_type, K_KEN_DRIVER_EXT_STR, EI_K_EXT_STR))
        return "driver";
    else if (!memcmp(k_type, K_KEN_MODULE_EXT_STR, EI_K_EXT_STR))
        return "module";
    return "Unknown";
}

std::vector<std::string> str_requested_loader_features(Elf64_Byte features)
{
    std::vector<std::string> out;
    if (features == K_LOADER_NO_LOADER_FEATURES)
    {
        return  { "K_LOADER_NO_LOADER_FEATURES" };
    }

    if (features & K_LOADER_MK_VMS)
    {
        out.emplace_back("K_LOADER_MK_VMS");
    }

    return out;
}

int main(int argc, const char** argv)
{
    po::options_description desc("glen_elf_tool options"s);
    desc.add_options()
    ("help,h", "produce help message")
    ("elf,e", po::value<std::string>()->value_name("path"s), "path to kernel elf (Required)")
    ("vid,v", po::value<Elf64_Half>()->value_name("kversion"s), "The kernel version ID")
    ("k-type,t", po::value<std::string>()->value_name("ktype"s), "kernel identification")
    ("loader-feature,f", po::value<std::vector<std::string>>()->composing()->value_name("feature"), "Used by the loader to perform special actions for the kernel")
    ("ignore-elf-magic,m", "Remove the requirement for elf magic")
    ("info,i", "Dumps info about the ELF");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.contains("help") || vm.empty())
    {
        help(desc);
        return 0;
    }

    if (!vm.contains("elf"))
    {
        cout_print("No no ELF binary provided\n");
        help(desc);
        return 1;
    }

    std::string elf_path = vm["elf"].as<std::string>();

    if (!std::filesystem::exists(elf_path))
    {
        std::println("ELF({}) file does not exist", elf_path);
        help(desc);
        return 1;
    }

    bool info_only = false;

    if (vm.contains("info"))
    {
        info_only = true;
    }

    Elf64_Byte loader_fet = 0;

    if (vm.contains("loader-feature") && !info_only)
    {
        for (const std::string& feat : vm["loader-feature"].as<std::vector<std::string>>())
        {
            Elf64_Byte feature_byte = to_load_feature(feat);
            if (feature_byte == K_LOADER_NO_LOADER_FEATURES)
            {
                loader_fet = 0;
                break;
            }
            loader_fet |= feature_byte;
        }
    }

    std::string ext_type;

    if (vm.contains("k-type") && !info_only)
    {
        try
        {
            ext_type = to_ext_str(vm["k-type"].as<std::string>());
        }
        catch (std::exception& ex)
        {
            std::println("{}", ex.what());
            return 1;
        }
    }

    Elf64_Half vid = 0;

    if (vm.contains("vid") && !info_only)
    {
        vid = vm["vid"].as<Elf64_Half>();
    }

    if ((vm.size() == 1 && vm.contains("elf")) ||
        (vm.size() == 2 && vm.contains("elf") && vm.contains("ignore-elf-magic"))
    ) {
        info_only = true;
    }

    std::FILE* elf_file = nullptr;
    if (info_only)
    {
        elf_file = std::fopen(elf_path.c_str(), "rb");
    }
    else
    {
        elf_file = std::fopen(elf_path.c_str(), "r+b");
    }

    if (!elf_file) {
        std::perror("fopen failed");
        return 1;
    }

    if (!vm.contains("ignore-elf-magic"))
    {
        std::uint8_t magic[4] = {0};
        if (std::fread(&magic, 1, sizeof(magic), elf_file) != sizeof(magic) ||
            magic[0] != 0x7F ||
            magic[1] != 'E'  ||
            magic[2] != 'L'  ||
            magic[3] != 'F')
        {
            std::println("ELF({}) Is not a valid ELF file", elf_path);
            std::fclose(elf_file);
            return 1;
        }
    }

    std::fseek(elf_file, offsetof(Elf64_Ident, fields.ext_elf_k_data), SEEK_SET);

    Elf64_Ext_Ident ext;
    std::memset(&ext, 0, sizeof(Elf64_Ext_Ident));

    std::fread(&ext, sizeof(Elf64_Ident), 1, elf_file);

    if (info_only)
    {
        auto kvid = ext.ext_fields.k_vid;
        std::println("kvid: {}", kvid);
        std::println("Requested Loader Features:");
        for (const std::string& feat : str_requested_loader_features(ext.ext_fields.k_requested_loader_feature_flags))
        {
            std::println("   {}", feat);
        }
        std::println("ktype: {}", str_k_type(ext.ext_fields.k_ext_str));
        std::fclose(elf_file);
        return 0;
    }

    if (vm.contains("vid"))
        ext.ext_fields.k_vid = vid;
    if (vm.contains("loader-feature"))
        ext.ext_fields.k_requested_loader_feature_flags = loader_fet;
    if (vm.contains("k-type"))
    {
        if (!ext_type.empty())
        {
            std::memcpy(ext.ext_fields.k_ext_str, ext_type.c_str(), EI_K_EXT_STR);
        }
    }


    std::fseek(elf_file, offsetof(Elf64_Ident, fields.ext_elf_k_data), SEEK_SET);
    std::fwrite(&ext, sizeof(Elf64_Ident), 1, elf_file);

    //perror("fopen failed");
    std::fclose(elf_file);
    return 0;
}
