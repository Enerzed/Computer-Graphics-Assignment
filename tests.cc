#include <gtest/gtest.h>
#include "KG121.h"
#include "KG121.cpp"

// Testing Init Function
TEST(InitTest, BuildTables) 
{
    // Prepare data
    const std::string filename = "sphere.obj";
    const double initial = -6000.0;
    IntervalScan intervalScan;

    // Action
    intervalScan.Init(filename, initial, 0.0);

    // Verify
    EXPECT_EQ(544, intervalScan.VT.size());     // Vertex table size
    EXPECT_EQ(3076, intervalScan.ET.size());    // Edge table size
    EXPECT_EQ(1024, intervalScan.FT.size());    // Face table size
    EXPECT_EQ(1026, intervalScan.PT.size());    // Poly table size
}


TEST(ScanTest, ImageNonChange)
{
	// Prepare data
	const std::string filename = "sphere.obj";
	const double initial = -6000.0;
	const double camera_x = 0.0;
	IntervalScan intervalScan;
	intervalScan.Init(filename, initial, camera_x);

	// Action & Verify
	Image image = intervalScan.Scan(true, Light(), false);
	EXPECT_EQ(WINDOW_WIDTH, image.getSize().x);
	EXPECT_EQ(WINDOW_HEIGHT, image.getSize().y);
}


// Test s2
TEST(MathTest, Square)
{
    // Prepare data
    float a = 3.0f;

	// Action & Verify
	EXPECT_FLOAT_EQ(9.0f, s2(a));
}


// Testing DotProduct
TEST(MathTest, DotProduct)
{
    // Prepare data
    Vector3f vec1(1.0f, 2.0f, 3.0f);
    Vector3f vec2(4.0f, 5.0f, 6.0f);

    // Action & Verify
    EXPECT_FLOAT_EQ(32.0f, DotProduct(vec1, vec2));

    // Prepare data for another test
    vec1.x = -3.0f;
    vec2.x = 4.0f;

    // Action & Verify
    EXPECT_FLOAT_EQ(16.0f, DotProduct(vec1, vec2));

    // Prepare data
    Vector3f vec3(0.0f, 0.0f, 0.0f);

    // Action & Verify
    EXPECT_FLOAT_EQ(0.0f, DotProduct(vec1, vec3));
}


// Testing CrossProduct
TEST(MathTest, CrossProduct)
{
    // Prepare data
    Vector3f vec1(1.0f, 2.0f, 3.0f);
    Vector3f vec2(4.0f, 5.0f, 6.0f);

    // Action & Verify
    EXPECT_EQ(Vector3f(-3.0f, 6.0f, -3.0f), CrossProduct(vec1, vec2));

    // Prepare data
    Vector3f vec3(1.0f, 2.0f, 3.0f);
    Vector3f vec4(2.0f, 4.0f, 6.0f);

    // Action & Verify
    EXPECT_EQ(Vector3f(0.0f, 0.0f, 0.0f), CrossProduct(vec3, vec4));
}


// Testing SolveABCD
TEST(IntervalScanMethods, SolveABCD)
{
    // Prepare data
    IntervalScan intervalScan;
    std::vector<Vector3f> VT;
    Indicies indicies;
    VT.push_back(Vector3f(1.0f, 2.0f, 3.0f));
    VT.push_back(Vector3f(4.0f, 5.0f, 6.0f));
    VT.push_back(Vector3f(7.0f, 8.0f, 10.0f));
    indicies.indicies.push_back(0);
	indicies.indicies.push_back(1);
	indicies.indicies.push_back(2);

    // Action
    Vec4 result = intervalScan.SolveABCD(VT, indicies);

    // Verify
	EXPECT_FLOAT_EQ(3, result.x);       // a
	EXPECT_FLOAT_EQ(-3, result.y);      // b
	EXPECT_FLOAT_EQ(0, result.z);       // c
	EXPECT_FLOAT_EQ(3, result.w);       // d
}


// Testing CalculateLightning
TEST(IntervalScanMethods, CalculateLightning)
{
    // Prepare data
    IntervalScan intervalScan;
    Vector3f vertex = Vector3f(1.0f, 2.0f, 3.0f);
    Vector3f normal = Vector3f(1.0f, 2.0f, 3.0f);
    Light light;
    light.intensity = Color::White;
	light.position = Vector3f(1.0f, 2.0f, 3.0f);

	// Action
	Color result = intervalScan.CalculateLightning(vertex, normal, light);

	// Verify
	EXPECT_EQ(Color::Black, result);
}


// Testing BarycentricInterpolation
TEST(IntervalScanMethods, BarycentricInterpolation) {
    // Prepare data
    IntervalScan intervalScan;
    Poly poly;

    poly.verticies = {
        Vector3f(0.0f, 0.0f, 0.0f),  // Vertex 0
        Vector3f(1.0f, 0.0f, 0.0f),  // Vertex 1
        Vector3f(0.0f, 1.0f, 0.0f)   // Vertex 2
    };

    poly.indicies_ = { 0, 1, 2 };

    std::vector<Color> colors = {
        Color(255, 0, 0),   // Red at vertex 0
        Color(0, 255, 0),   // Green at vertex 1
        Color(0, 0, 255)    // Blue at vertex 2
    };

    // Action & Verify
    Color result1 = intervalScan.BarycentricInterpolation(poly, colors, 0.0f, 0.0f);
    EXPECT_EQ(Color(255, 0, 0), result1);

    Color result2 = intervalScan.BarycentricInterpolation(poly, colors, 1.0f, 0.0f);
    EXPECT_EQ(Color(0, 255, 0), result2);

    Color result3 = intervalScan.BarycentricInterpolation(poly, colors, 0.0f, 1.0f);
    EXPECT_EQ(Color(0, 0, 255), result3);

    Color result4 = intervalScan.BarycentricInterpolation(poly, colors, 1.0f / 3.0f, 1.0f / 3.0f);
    EXPECT_EQ(Color(84, 85, 85), result4);

    Color result5 = intervalScan.BarycentricInterpolation(poly, colors, 2.0f, 2.0f);
    EXPECT_NE(Color(0, 0, 0), result5);
}


// Testing GetZ
TEST(PolygonMethonds, GetZ)
{
	// Prepare data
	Poly poly;
    poly.a = 1.0f;
	poly.b = 2.0f;
	poly.c = 3.0f;
	poly.d = 4.0f;

	// Action & Verify
	EXPECT_FLOAT_EQ(-2., poly.GetZ(0, 1));
	EXPECT_FLOAT_EQ(-7./3., poly.GetZ(1, 1));
	EXPECT_FLOAT_EQ(-8., poly.GetZ(2, 9));
}


// Testing CalculateNormal
TEST(PolygonMethods, CalculateNormal)
{
// Prepare data
	Poly poly;
	poly.a = 1.0f;
	poly.b = 2.0f;
	poly.c = 3.0f;
	poly.d = 4.0f;

    poly.verticies = {
    Vector3f(0.0f, 0.0f, 0.0f),  // Vertex 0
    Vector3f(1.0f, 0.0f, 0.0f),  // Vertex 1
    Vector3f(0.0f, 1.0f, 0.0f)   // Vertex 2
    };

    poly.indicies_ = { 0, 1, 2 };

    std::vector<Color> colors = {
        Color(255, 0, 0),   // Red at vertex 0
        Color(0, 255, 0),   // Green at vertex 1
        Color(0, 0, 255)    // Blue at vertex 2
    };

    poly.CalculateNormal();

	// Action & Verify
	EXPECT_FLOAT_EQ(0., poly.normal.x);
	EXPECT_FLOAT_EQ(0., poly.normal.y);
	EXPECT_FLOAT_EQ(1., poly.normal.z);
}