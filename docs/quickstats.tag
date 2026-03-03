<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>quickstats.hpp</name>
    <path>quickstats/</path>
    <filename>quickstats_8hpp.html</filename>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="class">
    <name>quickstats::MultipleQuantilesFixedNumber</name>
    <filename>classquickstats_1_1MultipleQuantilesFixedNumber.html</filename>
    <templarg>class Output_</templarg>
    <templarg>typename Number_</templarg>
    <member kind="function">
      <type></type>
      <name>MultipleQuantilesFixedNumber</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesFixedNumber.html</anchorfile>
      <anchor>a0362f8453eacfa548269f2e0a319179e</anchor>
      <arglist>(const Number_ num_obs, const Quantiles_ &amp;quantiles)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesFixedNumber.html</anchorfile>
      <anchor>aee940341006d12c6f5b8ca9c832e5121</anchor>
      <arglist>(Input_ *const ptr, OutputFun_ output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesFixedNumber.html</anchorfile>
      <anchor>a797ac599a2728f39b7ef4bbb8f63475c</anchor>
      <arglist>(const Number_ nnz, Input_ *const ptr, OutputFun_ output) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::MultipleQuantilesVariableNumber</name>
    <filename>classquickstats_1_1MultipleQuantilesVariableNumber.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Number_</templarg>
    <templarg>class QuantilesPointer_</templarg>
    <member kind="function">
      <type></type>
      <name>MultipleQuantilesVariableNumber</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesVariableNumber.html</anchorfile>
      <anchor>a9ab3e814a9df33cbc4e199ac149328bc</anchor>
      <arglist>(const Number_ max_num, QuantilesPointer_ quantiles_ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesVariableNumber.html</anchorfile>
      <anchor>a9520cbaf454851a6444030faac22a7aa</anchor>
      <arglist>(const Number_ num, Input_ *ptr, OutputFun_ output)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1MultipleQuantilesVariableNumber.html</anchorfile>
      <anchor>a8a77170c0a56f591be8271863c3a1e3e</anchor>
      <arglist>(const Number_ num, const Number_ nnz, Input_ *ptr, OutputFun_ output)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::SingleQuantileFixedNumber</name>
    <filename>classquickstats_1_1SingleQuantileFixedNumber.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Number_</templarg>
    <member kind="function">
      <type></type>
      <name>SingleQuantileFixedNumber</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>a618b718231dcec8c9d7aa91018a2449c</anchor>
      <arglist>(const Number_ num, const Output_ quantile)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>aeca16d5d48b528ec4131038ef3099875</anchor>
      <arglist>(Input_ *ptr) const</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>aed209822c087f3471c66dbc5bb3ec5ca</anchor>
      <arglist>(const Number_ nnz, Input_ *ptr) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::SingleQuantileVariableNumber</name>
    <filename>classquickstats_1_1SingleQuantileVariableNumber.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Number_</templarg>
    <member kind="function">
      <type></type>
      <name>SingleQuantileVariableNumber</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>ad16e2a3f21d410979a77c0edfb204ea1</anchor>
      <arglist>(Number_ max_num, const Output_ quantile)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>af44e3ce8db366b828cae2b07e6bee510</anchor>
      <arglist>(const Number_ num, Input_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>a13af43abca3637a7d9f80b624322bc9b</anchor>
      <arglist>(const Number_ num, const Number_ nnz, Input_ *ptr)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>quickstats</name>
    <filename>namespacequickstats.html</filename>
    <class kind="class">quickstats::MultipleQuantilesFixedNumber</class>
    <class kind="class">quickstats::MultipleQuantilesVariableNumber</class>
    <class kind="class">quickstats::SingleQuantileFixedNumber</class>
    <class kind="class">quickstats::SingleQuantileVariableNumber</class>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a739d1e42f6a0d27ad2e70b3464e54f81</anchor>
      <arglist>(const Number_ num, Input_ *const ptr)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>ab09b274d0b6be96d50c4a7dac2ff56c8</anchor>
      <arglist>(const Number_ num, const Number_ nnz, Input_ *const ptr)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Quickly compute simple statistics</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Quickly compute simple statistics">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
