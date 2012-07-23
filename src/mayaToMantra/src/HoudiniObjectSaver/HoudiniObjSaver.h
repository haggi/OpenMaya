#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace HoudiniObjectSaver {

	/// <summary>
	/// Zusammenfassung für HoudiniObjSaver
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class HoudiniObjSaver : public System::Windows::Forms::Form
	{
	public:
		HoudiniObjSaver(void)
		{
			InitializeComponent();
			//
			//TODO: Konstruktorcode hier hinzufügen.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~HoudiniObjSaver()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300,300);
			this->Text = L"HoudiniObjSaver";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
        }
#pragma endregion
	};
}
