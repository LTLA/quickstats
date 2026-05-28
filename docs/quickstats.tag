<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>median.hpp</name>
    <path>quickstats/</path>
    <filename>median_8hpp.html</filename>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>MultipleQuantiles.hpp</name>
    <path>quickstats/</path>
    <filename>MultipleQuantiles_8hpp.html</filename>
    <class kind="class">quickstats::MultipleQuantilesFixedNumber</class>
    <class kind="class">quickstats::MultipleQuantilesVariableNumber</class>
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
    <class kind="class">quickstats::RssRunningDense</class>
    <class kind="class">quickstats::RssRunningDenseSkip</class>
    <class kind="class">quickstats::RssRunningSparse</class>
    <class kind="class">quickstats::RssRunningSparseSkip</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>SingleQuantile.hpp</name>
    <path>quickstats/</path>
    <filename>SingleQuantile_8hpp.html</filename>
    <class kind="class">quickstats::SingleQuantileFixedNumber</class>
    <class kind="class">quickstats::SingleQuantileVariableNumber</class>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="file">
    <name>skip_values.hpp</name>
    <path>quickstats/</path>
    <filename>skip__values_8hpp.html</filename>
    <namespace>quickstats</namespace>
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
      <anchor>ac829ac1a51cf26656aa9a927abbfe0da</anchor>
      <arglist>(const std::size_t max_num_total, QuantilesPointer_ quantiles_ptr)</arglist>
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
  <compound kind="class">
    <name>quickstats::RssRunningDense</name>
    <filename>classquickstats_1_1RssRunningDense.html</filename>
    <templarg>typename Input_</templarg>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>RssRunningDense</name>
      <anchorfile>classquickstats_1_1RssRunningDense.html</anchorfile>
      <anchor>af67351fe4df54e9e2f59300b21c1b801</anchor>
      <arglist>(const std::size_t num_obj, Output_ *const mean, Output_ *const rss)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classquickstats_1_1RssRunningDense.html</anchorfile>
      <anchor>aba8cf27848bbb34173c8c26f7347b87f</anchor>
      <arglist>(const Input_ *const ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classquickstats_1_1RssRunningDense.html</anchorfile>
      <anchor>a5da0ea85bc271b3431d39f9a0f29005a</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::RssRunningDenseSkip</name>
    <filename>classquickstats_1_1RssRunningDenseSkip.html</filename>
    <templarg>typename Count_</templarg>
    <templarg>typename Input_</templarg>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>RssRunningDenseSkip</name>
      <anchorfile>classquickstats_1_1RssRunningDenseSkip.html</anchorfile>
      <anchor>a680dcce8b648666fca6715114277adc6</anchor>
      <arglist>(const std::size_t num_obj, Output_ *mean, Output_ *rss, Count_ *num_unskipped)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classquickstats_1_1RssRunningDenseSkip.html</anchorfile>
      <anchor>a78e0564ca83e9f7dc4220ac0d96e2521</anchor>
      <arglist>(const Input_ *ptr, Skip_ skip)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classquickstats_1_1RssRunningDenseSkip.html</anchorfile>
      <anchor>aa3b2033db1c7e38c0e62bc6bdf67f05f</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::RssRunningSparse</name>
    <filename>classquickstats_1_1RssRunningSparse.html</filename>
    <templarg>typename Count_</templarg>
    <templarg>typename Input_</templarg>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>RssRunningSparse</name>
      <anchorfile>classquickstats_1_1RssRunningSparse.html</anchorfile>
      <anchor>aee3ab861047492f1ac4b2caf27160b72</anchor>
      <arglist>(const std::size_t num_obj, Output_ *const mean, Output_ *const rss, Count_ *const num_non_zero)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classquickstats_1_1RssRunningSparse.html</anchorfile>
      <anchor>a558089cf538150e15c233016509f9f5c</anchor>
      <arglist>(const std::size_t num_non_zero_obs, const Input_ *const value, const Index_ *const index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classquickstats_1_1RssRunningSparse.html</anchorfile>
      <anchor>aefdcce4422dc638fa18b23fd0db3296f</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::RssRunningSparseSkip</name>
    <filename>classquickstats_1_1RssRunningSparseSkip.html</filename>
    <templarg>typename Count_</templarg>
    <templarg>typename Input_</templarg>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>RssRunningSparseSkip</name>
      <anchorfile>classquickstats_1_1RssRunningSparseSkip.html</anchorfile>
      <anchor>aadc49b939d0d88dd8de34c47bbb66e03</anchor>
      <arglist>(const std::size_t num_obj, Output_ *const mean, Output_ *const rss, Count_ *const num_non_zero, Count_ *const num_unskipped)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classquickstats_1_1RssRunningSparseSkip.html</anchorfile>
      <anchor>a8fe3f95a1aa52ccf19258c552ad3daf2</anchor>
      <arglist>(const std::size_t num_non_zero_obs, const Input_ *value, const Index_ *index, Skip_ skip)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classquickstats_1_1RssRunningSparseSkip.html</anchorfile>
      <anchor>a4318383804b3eebb640724320aee3277</anchor>
      <arglist>()</arglist>
    </member>
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
      <anchor>a958107e875e050011600e85cefbd22c9</anchor>
      <arglist>(const std::size_t max_num_total, const Output_ quantile)</arglist>
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
  <compound kind="namespace">
    <name>quickstats</name>
    <filename>namespacequickstats.html</filename>
    <class kind="class">quickstats::MultipleQuantilesFixedNumber</class>
    <class kind="class">quickstats::MultipleQuantilesVariableNumber</class>
    <class kind="struct">quickstats::RssResult</class>
    <class kind="class">quickstats::RssRunningDense</class>
    <class kind="class">quickstats::RssRunningDenseSkip</class>
    <class kind="class">quickstats::RssRunningSparse</class>
    <class kind="class">quickstats::RssRunningSparseSkip</class>
    <class kind="class">quickstats::SingleQuantileFixedNumber</class>
    <class kind="class">quickstats::SingleQuantileVariableNumber</class>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a66ea74c07fc1cb5a9a9689c8afc76123</anchor>
      <arglist>(const std::size_t num_total, Input_ *const ptr)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a2cb6e9679bbd56aba352d0eb680c1976</anchor>
      <arglist>(const std::size_t num_total, const std::size_t num_non_zero, Input_ *const values)</arglist>
    </member>
    <member kind="function">
      <type>RssResult&lt; Output_ &gt;</type>
      <name>rss</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a3ee422dd8fc8049526c27e64be4d5480</anchor>
      <arglist>(const std::size_t num_total, const std::size_t num_non_zero, const Input_ *const ptr)</arglist>
    </member>
    <member kind="function">
      <type>RssResult&lt; Output_ &gt;</type>
      <name>rss</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>ab18b547c9bf1f469e54292a2d345bacb</anchor>
      <arglist>(const std::size_t num_total, const Input_ *const ptr)</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>recenter_rss</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a18014e9467233e456a3b77b682d9a020</anchor>
      <arglist>(const std::size_t num_total, const Float_ old_rss, const Float_ old_mean, const Float_ new_mean)</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>skip_values</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a20deb047ec36e9fa0bc25dde40a3a8d3</anchor>
      <arglist>(const std::size_t num_total, Input_ *const ptr, Skip_ skip)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Quickly compute simple statistics</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Quickly compute simple statistics">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
