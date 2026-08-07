#include <gtest/gtest.h>
#include <librautils/utils.h>

// --- computeMean ---

TEST(LibrautilsMeanTest, OddCount)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 3.0e9};
    EXPECT_NEAR(librautils::computeMean(freqs), 2.0e9, 1.0);
}

TEST(LibrautilsMeanTest, EvenCount)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 3.0e9, 4.0e9};
    EXPECT_NEAR(librautils::computeMean(freqs), 2.5e9, 1.0);
}

TEST(LibrautilsMeanTest, SingleElement)
{
    std::vector<double> freqs = {5.0e9};
    EXPECT_NEAR(librautils::computeMean(freqs), 5.0e9, 1.0);
}

TEST(LibrautilsMeanTest, NonUniformSpacing)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 5.0e9};
    EXPECT_NEAR(librautils::computeMean(freqs), 8.0e9 / 3.0, 1.0);
}

// --- computeMedian ---

TEST(LibrautilsMedianTest, OddCount)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 3.0e9};
    EXPECT_NEAR(librautils::computeMedian(freqs), 2.0e9, 1.0);
}

TEST(LibrautilsMedianTest, EvenCount)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 3.0e9, 4.0e9};
    EXPECT_NEAR(librautils::computeMedian(freqs), 2.5e9, 1.0);
}

TEST(LibrautilsMedianTest, SingleElement)
{
    std::vector<double> freqs = {5.0e9};
    EXPECT_NEAR(librautils::computeMedian(freqs), 5.0e9, 1.0);
}

TEST(LibrautilsMedianTest, UnsortedInput)
{
    std::vector<double> freqs = {3.0e9, 1.0e9, 2.0e9};
    EXPECT_NEAR(librautils::computeMedian(freqs), 2.0e9, 1.0);
}

TEST(LibrautilsMedianTest, EvenNonUniform)
{
    // sorted: {1,3,5,9} -> (3+5)/2 = 4
    std::vector<double> freqs = {5.0e9, 1.0e9, 9.0e9, 3.0e9};
    EXPECT_NEAR(librautils::computeMedian(freqs), 4.0e9, 1.0);
}

// --- computeReferenceFrequency ---

TEST(LibrautilsRefFreqTest, MeanKeyword)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 3.0e9};
    double refFreqHz = 0;
    librautils::computeReferenceFrequency("mean", freqs, refFreqHz);
    EXPECT_NEAR(refFreqHz, 2.0e9, 1.0);
}

TEST(LibrautilsRefFreqTest, MedianKeyword)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 3.0e9};
    double refFreqHz = 0;
    librautils::computeReferenceFrequency("median", freqs, refFreqHz);
    EXPECT_NEAR(refFreqHz, 2.0e9, 1.0);
}

TEST(LibrautilsRefFreqTest, UserSpecifiedPassthrough)
{
    std::vector<double> freqs = {1.0e9, 2.0e9, 3.0e9};
    double refFreqHz = 0;
    std::string result = librautils::computeReferenceFrequency("1.4GHz", freqs, refFreqHz);
    EXPECT_EQ(result, "1.4GHz");
    EXPECT_EQ(refFreqHz, 0.0);
}
