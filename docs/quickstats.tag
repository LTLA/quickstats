<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>mad.hpp</name>
    <path>quickstats/</path>
    <filename>mad_8hpp.html</filename>
    <class kind="struct">quickstats::MadOptions</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>median.hpp</name>
    <path>quickstats/</path>
    <filename>median_8hpp.html</filename>
    <class kind="struct">quickstats::MedianOptions</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>MultipleQuantiles.hpp</name>
    <path>quickstats/</path>
    <filename>MultipleQuantiles_8hpp.html</filename>
    <class kind="class">quickstats::MultipleQuantilesFixedNumber</class>
    <class kind="struct">quickstats::MultipleQuantilesVariableNumberOptions</class>
    <class kind="class">quickstats::MultipleQuantilesVariableNumber</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>pairwise_sum.hpp</name>
    <path>quickstats/</path>
    <filename>pairwise__sum_8hpp.html</filename>
    <class kind="struct">quickstats::PairwiseSumWorkspace</class>
    <class kind="struct">quickstats::PairwiseSumOptions</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>quickstats.hpp</name>
    <path>quickstats/</path>
    <filename>quickstats_8hpp.html</filename>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>rss.hpp</name>
    <path>quickstats/</path>
    <filename>rss_8hpp.html</filename>
    <class kind="struct">quickstats::RssResult</class>
    <class kind="struct">quickstats::RssWorkspace</class>
    <class kind="struct">quickstats::RssOptions</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>SingleQuantile.hpp</name>
    <path>quickstats/</path>
    <filename>SingleQuantile_8hpp.html</filename>
    <class kind="class">quickstats::SingleQuantileFixedNumber</class>
    <class kind="struct">quickstats::SingleQuantileVariableNumberOptions</class>
    <class kind="class">quickstats::SingleQuantileVariableNumber</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>skip_values.hpp</name>
    <path>quickstats/</path>
    <filename>skip__values_8hpp.html</filename>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>utils.hpp</name>
    <path>quickstats/</path>
    <filename>utils_8hpp.html</filename>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="struct">
    <name>quickstats::MadOptions</name>
    <filename>structquickstats_1_1MadOptions.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_</type>
      <name>placeholder</name>
      <anchorfile>structquickstats_1_1MadOptions.html</anchorfile>
      <anchor>a67d07814c94afc0e8804c953249bb367</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>difference_between_infinities_is_zero</name>
      <anchorfile>structquickstats_1_1MadOptions.html</anchorfile>
      <anchor>a96520b114948e3486940759400fb3642</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>quickstats::MedianOptions</name>
    <filename>structquickstats_1_1MedianOptions.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_</type>
      <name>placeholder</name>
      <anchorfile>structquickstats_1_1MedianOptions.html</anchorfile>
      <anchor>ae8a75ffb923f5f67eab1ac9f306fa571</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::MultipleQuantilesFixedNumber</name>
    <filename>classquickstats_1_1MultipleQuantilesFixedNumber.html</filename>
    <templarg>class Output_</templarg>
    <member kind="function">
      <type></type>
      <name>MultipleQuantilesFixedNumber</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesFixedNumber.html</anchorfile>
      <anchor>afde114e935c5047ea3357e58e6ef6aec</anchor>
      <arglist>(const std::size_t num_total, const Quantiles_ &amp;quantiles)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesFixedNumber.html</anchorfile>
      <anchor>a6e87664d7a6a7e1b7c79ef8efd660206</anchor>
      <arglist>(Input_ *const ptr, OutputFun_ output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesFixedNumber.html</anchorfile>
      <anchor>af6a2a08c5a8cb6b992c1cbd06bc7eb59</anchor>
      <arglist>(const std::size_t num_non_zero, Input_ *const values, OutputFun_ output) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::MultipleQuantilesVariableNumber</name>
    <filename>classquickstats_1_1MultipleQuantilesVariableNumber.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>class QuantilesPointer_</templarg>
    <member kind="function">
      <type></type>
      <name>MultipleQuantilesVariableNumber</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesVariableNumber.html</anchorfile>
      <anchor>aacbe522c8d0eba2498475126da9f6c09</anchor>
      <arglist>(const std::size_t max_num_total, QuantilesPointer_ quantiles_ptr, const MultipleQuantilesVariableNumberOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesVariableNumber.html</anchorfile>
      <anchor>a43d12ab28f44695591eaaa368a1b5c97</anchor>
      <arglist>(const std::size_t num_total, Input_ *const ptr, OutputFun_ output)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesVariableNumber.html</anchorfile>
      <anchor>a109466cd8bfffe33ae837af631228aec</anchor>
      <arglist>(const std::size_t num_total, const std::size_t num_non_zero, Input_ *const values, OutputFun_ output)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>quickstats::MultipleQuantilesVariableNumberOptions</name>
    <filename>structquickstats_1_1MultipleQuantilesVariableNumberOptions.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_</type>
      <name>placeholder</name>
      <anchorfile>structquickstats_1_1MultipleQuantilesVariableNumberOptions.html</anchorfile>
      <anchor>a3840820cead7e9d81f481263b80222d5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>quickstats::PairwiseSumOptions</name>
    <filename>structquickstats_1_1PairwiseSumOptions.html</filename>
    <member kind="variable">
      <type>std::size_t</type>
      <name>max_sum_length</name>
      <anchorfile>structquickstats_1_1PairwiseSumOptions.html</anchorfile>
      <anchor>a1956f3fab11e685ce274009c6e1341fb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>quickstats::PairwiseSumWorkspace</name>
    <filename>structquickstats_1_1PairwiseSumWorkspace.html</filename>
    <templarg>typename Output_</templarg>
  </compound>
  <compound kind="struct">
    <name>quickstats::RssOptions</name>
    <filename>structquickstats_1_1RssOptions.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::size_t</type>
      <name>max_sum_length</name>
      <anchorfile>structquickstats_1_1RssOptions.html</anchorfile>
      <anchor>a7e2d0ab42f405e31a3bba93a9b03e73a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Output_</type>
      <name>mean_placeholder</name>
      <anchorfile>structquickstats_1_1RssOptions.html</anchorfile>
      <anchor>a4a8efeb7c4500556892c059be38720c2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>quickstats::RssResult</name>
    <filename>structquickstats_1_1RssResult.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_</type>
      <name>mean</name>
      <anchorfile>structquickstats_1_1RssResult.html</anchorfile>
      <anchor>ab025af92ba44f996d3dc19e074372d2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Output_</type>
      <name>rss</name>
      <anchorfile>structquickstats_1_1RssResult.html</anchorfile>
      <anchor>a3c99a0a61c93b092c1fa0b83ab0bed27</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>quickstats::RssWorkspace</name>
    <filename>structquickstats_1_1RssWorkspace.html</filename>
    <templarg>typename Output_</templarg>
  </compound>
  <compound kind="class">
    <name>quickstats::SingleQuantileFixedNumber</name>
    <filename>classquickstats_1_1SingleQuantileFixedNumber.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>SingleQuantileFixedNumber</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>ad449cd259f6ec320afcaa00f013bb388</anchor>
      <arglist>(const std::size_t num_total, const Output_ quantile)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>abb23c362ce91af9b60fec8e44fdb6261</anchor>
      <arglist>(Input_ *const ptr) const</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>ab58ccd5925a27793969c547b69775d7e</anchor>
      <arglist>(const std::size_t num_non_zero, Input_ *const ptr) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::SingleQuantileVariableNumber</name>
    <filename>classquickstats_1_1SingleQuantileVariableNumber.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>SingleQuantileVariableNumber</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>ade331f2872a09100339528caa330623f</anchor>
      <arglist>(const std::size_t max_num_total, const Output_ quantile, const SingleQuantileVariableNumberOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>ad64ce9931898583f14e16937a88b2e4c</anchor>
      <arglist>(const std::size_t num_total, Input_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>a6cc57997a7c5a6c2b361a7eca5dfff90</anchor>
      <arglist>(const std::size_t num_total, const std::size_t num_non_zero, Input_ *const values)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>quickstats::SingleQuantileVariableNumberOptions</name>
    <filename>structquickstats_1_1SingleQuantileVariableNumberOptions.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_</type>
      <name>placeholder</name>
      <anchorfile>structquickstats_1_1SingleQuantileVariableNumberOptions.html</anchorfile>
      <anchor>a68ed97e32770492b9bde541911a10ca2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>quickstats</name>
    <filename>namespacequickstats.html</filename>
    <class kind="struct">quickstats::MadOptions</class>
    <class kind="struct">quickstats::MedianOptions</class>
    <class kind="class">quickstats::MultipleQuantilesFixedNumber</class>
    <class kind="class">quickstats::MultipleQuantilesVariableNumber</class>
    <class kind="struct">quickstats::MultipleQuantilesVariableNumberOptions</class>
    <class kind="struct">quickstats::PairwiseSumOptions</class>
    <class kind="struct">quickstats::PairwiseSumWorkspace</class>
    <class kind="struct">quickstats::RssOptions</class>
    <class kind="struct">quickstats::RssResult</class>
    <class kind="struct">quickstats::RssWorkspace</class>
    <class kind="class">quickstats::SingleQuantileFixedNumber</class>
    <class kind="class">quickstats::SingleQuantileVariableNumber</class>
    <class kind="struct">quickstats::SingleQuantileVariableNumberOptions</class>
    <member kind="function">
      <type>Output_</type>
      <name>mad</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a5f49679389644f302b7bbf897277d49a</anchor>
      <arglist>(const std::size_t num_total, Input_ *const ptr, const Input_ median, const MadOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>mad</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>af2970a9cedcef7fcfa34f8452dfbd816</anchor>
      <arglist>(const std::size_t num_total, const std::size_t num_non_zero, Input_ *const values, const Input_ median, const MadOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>scale_mad_to_sd</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a294a3815731fe4eccb7b4d5fb13228b6</anchor>
      <arglist>(const Float_ x)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a0c665cf3d1d2e73842221509a6586f9d</anchor>
      <arglist>(const std::size_t num_total, Input_ *const ptr, const MedianOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>ae2b005b0621b67993c5a6661aa0f1e77</anchor>
      <arglist>(const std::size_t num_total, const std::size_t num_non_zero, Input_ *const values, const MedianOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>pairwise_sum_abstract</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a1d2ec86f47fbbd6fa6e39ff62b957e34</anchor>
      <arglist>(const std::size_t num_total, Input_ input, PairwiseSumWorkspace&lt; Output_ &gt; &amp;work, const PairwiseSumOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>pairwise_sum</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>ae04364a3ca1c759613d54688f1799adc</anchor>
      <arglist>(const std::size_t num_total, const Input_ *const ptr, PairwiseSumWorkspace&lt; Output_ &gt; &amp;work, const PairwiseSumOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>RssResult&lt; Output_ &gt;</type>
      <name>rss</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a3e10f8d0df934aea3fe37799d5fb1dd4</anchor>
      <arglist>(const std::size_t num_total, const std::size_t num_non_zero, const Input_ *const ptr, RssWorkspace&lt; Output_ &gt; &amp;work, const RssOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>RssResult&lt; Output_ &gt;</type>
      <name>rss</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>afe323ae5e6eca125513c9d72dd795c81</anchor>
      <arglist>(const std::size_t num_total, const Input_ *const ptr, RssWorkspace&lt; Output_ &gt; &amp;work, const RssOptions&lt; Output_ &gt; &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_rss</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>aeec4c4524e73f23911b5a4430c184286</anchor>
      <arglist>(Output_ &amp;mean, Output_ &amp;rss, const Input_ value, const Count_ num_total)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_rss_with_zeros_unsafe</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a7a1f48b44e913f31abcc368e96b19a57</anchor>
      <arglist>(Output_ &amp;mean, Output_ &amp;rss, const Count_ num_zeros, const Count_ num_total)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_rss_with_zeros</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>ae0771a2a6576095f4e40e8b12fbd7fea</anchor>
      <arglist>(Output_ &amp;mean, Output_ &amp;rss, const Count_ num_zeros, const Count_ num_total)</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>recenter_rss_unsafe</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a4a91f2b10a73a7f42d8e603a9edc9690</anchor>
      <arglist>(const Count_ num_total, const Float_ old_rss, const Float_ old_mean, const Float_ new_mean)</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>recenter_rss</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a4ec3230924c4786e9176f7634c844d97</anchor>
      <arglist>(const Count_ num_total, const Float_ old_rss, const Float_ old_mean, const Float_ new_mean)</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>skip_values</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a20deb047ec36e9fa0bc25dde40a3a8d3</anchor>
      <arglist>(const std::size_t num_total, Input_ *const ptr, Skip_ skip)</arglist>
    </member>
    <member kind="function">
      <type>constexpr Value_</type>
      <name>nan_if_available_else_zero</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>addf99f3c0f16a9bb7690e801ab33f84c</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Quickly compute simple statistics</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
