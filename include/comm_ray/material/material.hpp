#pragma once

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "ray.hpp"
#include "utils.hpp"
#include "glm/glm.hpp"
#include "intersect_record.hpp"
#include <cmath>

namespace SignalTracer {
    class IntersectRecord;

    class Material {
    public:
        Material() = default;

        // Material(const Material& other) = delete;
        // Material(Material&& other) = delete;
        // Material& operator=(const Material& other) = delete;
        // Material& operator=(Material&& other) = delete;
        virtual ~Material() = default;

        virtual std::ostream& print(std::ostream& out) const {
            return out;
        }
        friend std::ostream& operator<<(std::ostream& out, const Material& material) {
            return material.print(out);
            // << "\tReal relative permittivity, a = " << material.get_real_relative_permittivity_a() << ", b = " << material.get_real_relative_permittivity_b() << "\n" << "\tConductivity, S/m, c = " << material.get_conductivity_c() << ", d = " << material.get_conductivity_d() << "\n";
        }

        // perfect reflection
        virtual bool is_scattering(const Ray& ray_in, const IntersectRecord& record, glm::vec3& attenuation, Ray& scattered_ray) const {
            attenuation = glm::vec3{ 1.0, 1.0, 1.0 };

            glm::vec3 normal{ record.normal };
            glm::vec3 scattered_direction{ reflect(ray_in.get_direction(), normal) };

            scattered_ray = Ray(record.point, scattered_direction);
            return true;
        }

        // virtual bool is_scattering(const Ray& ray_in, const IntersectRecord& record, glm::vec3& attenuation, Ray& scattered_ray) const {
        //     attenuation = glm::vec3{ 1.0, 1.0, 1.0 };

        //     glm::vec3 normal{ record.get_normal() };
        //     glm::vec3 scattered_direction{ reflect(ray_in.get_direction(), normal) };

        //     scattered_ray = Ray(record.get_point(), scattered_direction);
        //     return true;
        // }

        glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) const {
            return v - 2 * glm::dot(v, n) * n;
        }

        void set_real_relative_permittivity_a(float real_relative_permittivity_a) {
            m_real_relative_permittivity_a = real_relative_permittivity_a;
        }

        void set_real_relative_permittivity_b(float real_relative_permittivity_b) {
            m_real_relative_permittivity_b = real_relative_permittivity_b;
        }

        void set_conductivity_c(float conductivity_c) {
            m_conductivity_c = conductivity_c;
        }

        void set_conductivity_d(float conductivity_d) {
            m_conductivity_d = conductivity_d;
        }

        float get_real_relative_permittivity_a() const {
            return m_real_relative_permittivity_a;
        }

        float get_real_relative_permittivity_b() const {
            return m_real_relative_permittivity_b;
        }

        float get_conductivity_c() const {
            return m_conductivity_c;
        }

        float get_conductivity_d() const {
            return m_conductivity_d;
        }

        /// @brief Calculate the conductivity of the material. Formula: c*f^d
        /// @param frequency Hz
        /// @return Conductivity, S/m
        float calc_conductivity(float frequency) const {
            return m_conductivity_c * pow(frequency, m_conductivity_d);
        }

        /// @brief Calculate the real part of the relative permittivity of the material. Formula: a*f^b
        /// @param frequency Hz
        /// @return real relative permittivity, unitless
        float calc_real_relative_permittivity(float frequency) const {
            return m_real_relative_permittivity_a * pow(frequency, m_real_relative_permittivity_b);
        }

        /**
        @brief Calculate the complex part of the relative permittivity of the material. Formula: conductivity / (2*pi*electric_constant*frequency). \n
        Elecrtic constant: 8.854187817e-12 F/m
        @param frequency Hz
        @return complex relative permittivity, unitless
        */
        float calc_imaginary_relative_permittivity(float frequency) const {
            float conductivity = calc_conductivity(frequency);
            float electric_constant = 8.854187817e-12f; // F/m
            return conductivity / (2.0f * M_PI * electric_constant * frequency);
        }


    private:
        float m_real_relative_permittivity_a{ 1 };
        float m_real_relative_permittivity_b{ 0 };
        float m_conductivity_c{ 0 }; // S/m
        float m_conductivity_d{ 0 }; // S/m
    };

    /// @brief Material properties for air with frequency being in the range of [0.001, 100] GHz.
    class Air : public Material {
    public:
        Air() {
            set_real_relative_permittivity_a(1.0f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0f);
            set_conductivity_d(0.0f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Air.\n";
            return out;
        }
    };

    /// @brief Material properties for concrete with frequency being in the range of [1, 100] GHz.
    class Concrete : public Material {
    public:
        Concrete() {
            set_real_relative_permittivity_a(5.31f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0326f);
            set_conductivity_d(0.8095f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Concrete.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [1, 10] GHz.
    class Brick : public Material {
    public:
        Brick() {
            set_real_relative_permittivity_a(3.75f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.038f);
            set_conductivity_d(0.0f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Brick.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [1, 100] GHz.
    class Plasterboard : public Material {
    public:
        Plasterboard() {
            set_real_relative_permittivity_a(2.94f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0116f);
            set_conductivity_d(0.7076f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Plasterboard.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [0.001, 100] GHz.
    class Wood : public Material {
    public:
        Wood() {
            set_real_relative_permittivity_a(1.99f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0047f);
            set_conductivity_d(1.0718f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Wood.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [0.01, 100] GHz.
    class Glass : public Material {
    public:
        Glass() {
            set_real_relative_permittivity_a(6.27f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0043f);
            set_conductivity_d(1.1925f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Glass.\n";
            return out;
        }
    };

    /// @brief Material properties for ceiling board with frequency being in the range of [1, 100] GHz.
    class CeilingBoard : public Material {
    public:
        CeilingBoard() {
            set_real_relative_permittivity_a(1.50f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0005f);
            set_conductivity_d(1.1634f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "CeilingBoard.\n";
            return out;
        }
    };

    /// @brief Material properties for chipboard with frequency being in the range of [1, 100] GHz.
    class Chipboard : public Material {
    public:
        Chipboard() {
            set_real_relative_permittivity_a(2.58f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0217f);
            set_conductivity_d(0.78f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Chipboard.\n";
            return out;
        }
    };

    /// @brief Material properties for floorboard with frequency being in the range of [50, 100] GHz.
    class Floorboard : public Material {
    public:
        Floorboard() {
            set_real_relative_permittivity_a(3.66f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.0044f);
            set_conductivity_d(1.3515f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Floorboard.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [1, 100] GHz.
    class Metal : public Material {
    public:
        Metal() {
            set_real_relative_permittivity_a(1.0f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(1.0e7f);
            set_conductivity_d(0.0f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "Metal.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [1, 10] GHz.
    class VeryDryGround : public Material {
    public:
        VeryDryGround() {
            set_real_relative_permittivity_a(3.0f);
            set_real_relative_permittivity_b(0.0f);
            set_conductivity_c(0.00015f);
            set_conductivity_d(2.52f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "VeryDryGround.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [1, 10] GHz.
    class MediumDryGround : public Material {
    public:
        MediumDryGround() {
            set_real_relative_permittivity_a(15.0f);
            set_real_relative_permittivity_b(-0.1f);
            set_conductivity_c(0.035f);
            set_conductivity_d(1.63f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "MediumDryGround.\n";
            return out;
        }
    };

    /// @brief Material properties for glass with frequency being in the range of [1, 10] GHz.
    class WetGround : public Material {
    public:
        WetGround() {
            set_real_relative_permittivity_a(30.0f);
            set_real_relative_permittivity_b(-0.4f);
            set_conductivity_c(0.15f);
            set_conductivity_d(1.30f);
        }

        std::ostream& print(std::ostream& out) const override {
            out << "WetGround.\n";
            return out;
        }
    };
}

#endif // !MATERIAL_HPP