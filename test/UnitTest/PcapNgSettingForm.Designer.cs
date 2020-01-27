namespace MT
{
	partial class PcapNgSettingForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.GrpLanIFIPAddress = new System.Windows.Forms.GroupBox();
			this.UnitName = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.UDPPort = new System.Windows.Forms.TextBox();
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.dot1 = new System.Windows.Forms.Label();
			this.dot2 = new System.Windows.Forms.Label();
			this.IP1 = new System.Windows.Forms.TextBox();
			this.IP2 = new System.Windows.Forms.TextBox();
			this.IP3 = new System.Windows.Forms.TextBox();
			this.dot3 = new System.Windows.Forms.Label();
			this.IP4 = new System.Windows.Forms.TextBox();
			this.Label23 = new System.Windows.Forms.Label();
			this.Label25 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.dataGridView1 = new System.Windows.Forms.DataGridView();
			this.numberDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.accessNameDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.checkDataGridViewCheckBoxColumn = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.portNoDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.settingItemBindingSource = new System.Windows.Forms.BindingSource(this.components);
			this.OKButton = new System.Windows.Forms.Button();
			this.CButton = new System.Windows.Forms.Button();
			this.GrpLanIFIPAddress.SuspendLayout();
			this.tableLayoutPanel1.SuspendLayout();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.settingItemBindingSource)).BeginInit();
			this.SuspendLayout();
			// 
			// GrpLanIFIPAddress
			// 
			this.GrpLanIFIPAddress.Controls.Add(this.UnitName);
			this.GrpLanIFIPAddress.Controls.Add(this.label1);
			this.GrpLanIFIPAddress.Controls.Add(this.UDPPort);
			this.GrpLanIFIPAddress.Controls.Add(this.tableLayoutPanel1);
			this.GrpLanIFIPAddress.Controls.Add(this.Label23);
			this.GrpLanIFIPAddress.Controls.Add(this.Label25);
			this.GrpLanIFIPAddress.Location = new System.Drawing.Point(12, 3);
			this.GrpLanIFIPAddress.Name = "GrpLanIFIPAddress";
			this.GrpLanIFIPAddress.Size = new System.Drawing.Size(382, 71);
			this.GrpLanIFIPAddress.TabIndex = 5;
			this.GrpLanIFIPAddress.TabStop = false;
			this.GrpLanIFIPAddress.Text = "装置側設定";
			// 
			// UnitName
			// 
			this.UnitName.Location = new System.Drawing.Point(209, 41);
			this.UnitName.MaxLength = 7;
			this.UnitName.Name = "UnitName";
			this.UnitName.Size = new System.Drawing.Size(62, 19);
			this.UnitName.TabIndex = 14;
			this.UnitName.Text = "LAN-IF";
			this.UnitName.TextChanged += new System.EventHandler(this.UnitName_TextChanged);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(162, 46);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(41, 12);
			this.label1.TabIndex = 13;
			this.label1.Text = "装置名";
			// 
			// UDPPort
			// 
			this.UDPPort.Location = new System.Drawing.Point(67, 42);
			this.UDPPort.Name = "UDPPort";
			this.UDPPort.Size = new System.Drawing.Size(62, 19);
			this.UDPPort.TabIndex = 12;
			this.UDPPort.Text = "11001";
			this.UDPPort.TextChanged += new System.EventHandler(this.UDPPort_TextChanged);
			this.UDPPort.Enter += new System.EventHandler(this.TextBox_Enter);
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.BackColor = System.Drawing.SystemColors.Window;
			this.tableLayoutPanel1.ColumnCount = 7;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 25F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 15F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 25F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 15F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 25F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 15F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 25F));
			this.tableLayoutPanel1.Controls.Add(this.dot1, 1, 0);
			this.tableLayoutPanel1.Controls.Add(this.dot2, 3, 0);
			this.tableLayoutPanel1.Controls.Add(this.IP1, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.IP2, 2, 0);
			this.tableLayoutPanel1.Controls.Add(this.IP3, 4, 0);
			this.tableLayoutPanel1.Controls.Add(this.dot3, 5, 0);
			this.tableLayoutPanel1.Controls.Add(this.IP4, 6, 0);
			this.tableLayoutPanel1.Cursor = System.Windows.Forms.Cursors.Default;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(67, 18);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 1;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Size = new System.Drawing.Size(145, 17);
			this.tableLayoutPanel1.TabIndex = 11;
			// 
			// dot1
			// 
			this.dot1.BackColor = System.Drawing.SystemColors.Window;
			this.dot1.Enabled = false;
			this.dot1.ForeColor = System.Drawing.SystemColors.MenuText;
			this.dot1.Location = new System.Drawing.Point(28, 0);
			this.dot1.Name = "dot1";
			this.dot1.Size = new System.Drawing.Size(9, 17);
			this.dot1.TabIndex = 7;
			this.dot1.Text = ".";
			this.dot1.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
			// 
			// dot2
			// 
			this.dot2.BackColor = System.Drawing.SystemColors.Window;
			this.dot2.Enabled = false;
			this.dot2.ForeColor = System.Drawing.SystemColors.WindowText;
			this.dot2.Location = new System.Drawing.Point(68, 0);
			this.dot2.Name = "dot2";
			this.dot2.Size = new System.Drawing.Size(9, 17);
			this.dot2.TabIndex = 9;
			this.dot2.Text = ".";
			this.dot2.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
			// 
			// IP1
			// 
			this.IP1.BackColor = System.Drawing.SystemColors.Window;
			this.IP1.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.IP1.ForeColor = System.Drawing.SystemColors.WindowText;
			this.IP1.Location = new System.Drawing.Point(3, 3);
			this.IP1.MaxLength = 3;
			this.IP1.Name = "IP1";
			this.IP1.Size = new System.Drawing.Size(19, 12);
			this.IP1.TabIndex = 1;
			this.IP1.Text = "192";
			this.IP1.TextChanged += new System.EventHandler(this.IPCheck);
			this.IP1.Enter += new System.EventHandler(this.TextBox_Enter);
			// 
			// IP2
			// 
			this.IP2.BackColor = System.Drawing.SystemColors.Window;
			this.IP2.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.IP2.ForeColor = System.Drawing.SystemColors.WindowText;
			this.IP2.Location = new System.Drawing.Point(43, 3);
			this.IP2.MaxLength = 3;
			this.IP2.Name = "IP2";
			this.IP2.Size = new System.Drawing.Size(19, 12);
			this.IP2.TabIndex = 8;
			this.IP2.Text = "168";
			this.IP2.TextChanged += new System.EventHandler(this.IPCheck);
			this.IP2.Enter += new System.EventHandler(this.TextBox_Enter);
			// 
			// IP3
			// 
			this.IP3.BackColor = System.Drawing.SystemColors.Window;
			this.IP3.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.IP3.ForeColor = System.Drawing.SystemColors.WindowText;
			this.IP3.Location = new System.Drawing.Point(83, 3);
			this.IP3.MaxLength = 3;
			this.IP3.Name = "IP3";
			this.IP3.Size = new System.Drawing.Size(19, 12);
			this.IP3.TabIndex = 10;
			this.IP3.Text = "10";
			this.IP3.TextChanged += new System.EventHandler(this.IPCheck);
			this.IP3.Enter += new System.EventHandler(this.TextBox_Enter);
			// 
			// dot3
			// 
			this.dot3.BackColor = System.Drawing.SystemColors.Window;
			this.dot3.Enabled = false;
			this.dot3.ForeColor = System.Drawing.SystemColors.WindowText;
			this.dot3.Location = new System.Drawing.Point(108, 0);
			this.dot3.Name = "dot3";
			this.dot3.Size = new System.Drawing.Size(9, 17);
			this.dot3.TabIndex = 11;
			this.dot3.Text = ".";
			this.dot3.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
			// 
			// IP4
			// 
			this.IP4.BackColor = System.Drawing.SystemColors.Window;
			this.IP4.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.IP4.ForeColor = System.Drawing.SystemColors.WindowText;
			this.IP4.Location = new System.Drawing.Point(123, 3);
			this.IP4.MaxLength = 3;
			this.IP4.Name = "IP4";
			this.IP4.Size = new System.Drawing.Size(19, 12);
			this.IP4.TabIndex = 12;
			this.IP4.Text = "201";
			this.IP4.TextChanged += new System.EventHandler(this.IPCheck);
			this.IP4.Enter += new System.EventHandler(this.TextBox_Enter);
			// 
			// Label23
			// 
			this.Label23.AutoSize = true;
			this.Label23.Location = new System.Drawing.Point(8, 20);
			this.Label23.Name = "Label23";
			this.Label23.Size = new System.Drawing.Size(53, 12);
			this.Label23.TabIndex = 0;
			this.Label23.Text = "ＩＰアドレス";
			// 
			// Label25
			// 
			this.Label25.AutoSize = true;
			this.Label25.Location = new System.Drawing.Point(8, 46);
			this.Label25.Name = "Label25";
			this.Label25.Size = new System.Drawing.Size(56, 12);
			this.Label25.TabIndex = 2;
			this.Label25.Text = "UDPポート";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.dataGridView1);
			this.groupBox1.Location = new System.Drawing.Point(12, 80);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(382, 389);
			this.groupBox1.TabIndex = 6;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "接続先設定";
			// 
			// dataGridView1
			// 
			this.dataGridView1.AutoGenerateColumns = false;
			this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.numberDataGridViewTextBoxColumn,
            this.accessNameDataGridViewTextBoxColumn,
            this.checkDataGridViewCheckBoxColumn,
            this.portNoDataGridViewTextBoxColumn});
			this.dataGridView1.DataSource = this.settingItemBindingSource;
			this.dataGridView1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView1.Location = new System.Drawing.Point(3, 15);
			this.dataGridView1.Name = "dataGridView1";
			this.dataGridView1.RowTemplate.Height = 21;
			this.dataGridView1.Size = new System.Drawing.Size(376, 371);
			this.dataGridView1.TabIndex = 0;
			// 
			// numberDataGridViewTextBoxColumn
			// 
			this.numberDataGridViewTextBoxColumn.DataPropertyName = "Number";
			this.numberDataGridViewTextBoxColumn.HeaderText = "番号";
			this.numberDataGridViewTextBoxColumn.MinimumWidth = 40;
			this.numberDataGridViewTextBoxColumn.Name = "numberDataGridViewTextBoxColumn";
			this.numberDataGridViewTextBoxColumn.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.numberDataGridViewTextBoxColumn.Width = 40;
			// 
			// accessNameDataGridViewTextBoxColumn
			// 
			this.accessNameDataGridViewTextBoxColumn.DataPropertyName = "AccessName";
			this.accessNameDataGridViewTextBoxColumn.HeaderText = "名前";
			this.accessNameDataGridViewTextBoxColumn.MaxInputLength = 7;
			this.accessNameDataGridViewTextBoxColumn.MinimumWidth = 40;
			this.accessNameDataGridViewTextBoxColumn.Name = "accessNameDataGridViewTextBoxColumn";
			this.accessNameDataGridViewTextBoxColumn.Width = 40;
			// 
			// checkDataGridViewCheckBoxColumn
			// 
			this.checkDataGridViewCheckBoxColumn.DataPropertyName = "Check";
			this.checkDataGridViewCheckBoxColumn.HeaderText = "有効";
			this.checkDataGridViewCheckBoxColumn.MinimumWidth = 40;
			this.checkDataGridViewCheckBoxColumn.Name = "checkDataGridViewCheckBoxColumn";
			this.checkDataGridViewCheckBoxColumn.Width = 40;
			// 
			// portNoDataGridViewTextBoxColumn
			// 
			this.portNoDataGridViewTextBoxColumn.DataPropertyName = "PortNo";
			this.portNoDataGridViewTextBoxColumn.HeaderText = "ポート番号";
			this.portNoDataGridViewTextBoxColumn.MinimumWidth = 40;
			this.portNoDataGridViewTextBoxColumn.Name = "portNoDataGridViewTextBoxColumn";
			// 
			// settingItemBindingSource
			// 
			this.settingItemBindingSource.DataSource = typeof(MT.AccessPoint);
			// 
			// OKButton
			// 
			this.OKButton.Location = new System.Drawing.Point(226, 490);
			this.OKButton.Name = "OKButton";
			this.OKButton.Size = new System.Drawing.Size(75, 23);
			this.OKButton.TabIndex = 7;
			this.OKButton.Text = "OK";
			this.OKButton.UseVisualStyleBackColor = true;
			this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
			// 
			// CButton
			// 
			this.CButton.Location = new System.Drawing.Point(319, 490);
			this.CButton.Name = "CButton";
			this.CButton.Size = new System.Drawing.Size(75, 23);
			this.CButton.TabIndex = 8;
			this.CButton.Text = "キャンセル";
			this.CButton.UseVisualStyleBackColor = true;
			this.CButton.Click += new System.EventHandler(this.CancelButton_Click);
			// 
			// PcapNgSettingForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(407, 525);
			this.Controls.Add(this.CButton);
			this.Controls.Add(this.OKButton);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.GrpLanIFIPAddress);
			this.Name = "PcapNgSettingForm";
			this.Text = "SettingForm";
			this.Load += new System.EventHandler(this.SettingForm_Load);
			this.GrpLanIFIPAddress.ResumeLayout(false);
			this.GrpLanIFIPAddress.PerformLayout();
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel1.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.settingItemBindingSource)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion
		private System.Windows.Forms.BindingSource settingItemBindingSource;
		private System.Windows.Forms.GroupBox GrpLanIFIPAddress;
		private System.Windows.Forms.Label Label23;
		private System.Windows.Forms.Label Label25;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button OKButton;
		private System.Windows.Forms.Button CButton;
		private System.Windows.Forms.Label dot2;
		private System.Windows.Forms.Label dot1;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
		private System.Windows.Forms.Label dot3;
		private System.Windows.Forms.TextBox UDPPort;
		public System.Windows.Forms.TextBox IP1;
		public System.Windows.Forms.TextBox IP2;
		public System.Windows.Forms.TextBox IP3;
		public System.Windows.Forms.TextBox IP4;
		private System.Windows.Forms.DataGridView dataGridView1;
		private System.Windows.Forms.TextBox UnitName;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.DataGridViewTextBoxColumn numberDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn accessNameDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewCheckBoxColumn checkDataGridViewCheckBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn iPAddrStrDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn portNoDataGridViewTextBoxColumn;
	}
}