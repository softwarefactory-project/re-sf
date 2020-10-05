open Jest;
open Tenant;
open Connection;
open Project;
open Resources;

describe("Test resources JSON parsing", () => {
  Expect.(
    test("parse tenant object", () => {
      let json = {|
        {
          "default-connection": "rdoproject.org",
          "description": "The rdoproject.org tenant",
          "tenant-options": {
              "zuul/report-build-page": true,
              "zuul/web-root": "https://review.rdoproject.org/zuul/"
          },
          "url": "https://review.rdoproject.org/manage",
          "name": "rdoproject.org"
        }
      |};
      let expected = {
        name: "rdoproject.org",
        default_connection: Some("rdoproject.org"),
        description: Some("The rdoproject.org tenant"),
        url: "https://review.rdoproject.org/manage",
      };

      let tenant = Tenant.parse(Json.parseOrRaise(json));
      expect(tenant.name) |> toBe(expected.name);
    })
  );

  Expect.(
    test("parse connection object", () => {
      let json = {|
        {
          "base-url": "https://pagure.io",
          "type": "pagure",
          "name": "pagure.io"
        }
      |};
      let expected = {
        base_url: Some("https://pagure.io"),
        connection_type: "pagure",
        name: "pagure.io",
      };
      let parsed = Connection.parse(Json.parseOrRaise(json));
      expect(parsed.name) |> toBe(expected.name);
    })
  );

  Expect.(
    test("parse project object", () => {
      let json = {|
        {
          "contacts": [
            "harrymichal@seznam.cz"
          ],
          "description": "Unprivileged development environment",
          "tenant": "local",
          "website": "https://github.com/debarshiray/toolbox",
          "name": "toolbox",
          "source-repositories": [
            {
              "containers/toolbox": {
                "connection": "github.com",
                "zuul/exclude-unprotected-branches": true
              }
            },
            "software-factory/cauth",
            {
              "software-factory/managesf": {}
            }
          ]
        }
      |};
      let expected = {
        name: "toolbox",
        contacts: Some(["harrymichal@seznam.cz"]),
        description: "Unprivileged development environment",
        tenant: Some("local"),
        website: Some("https://github.com/debarshiray/toolbox"),
        issue_tracker_url: None,
        review_dashboard: None,
        mailing_lists: Some([]),
        connection: None,
        documentation: None,
        options: Some([]),
        source_repositories: [
          {name: "containers/toolbox", connection: Some("github.com")},
          {name: "software-factory/cauth", connection: None},
          {name: "software-factory/managesf", connection: None},
        ],
      };
      let parsed = Project.parse(Json.parseOrRaise(json));
      expect(parsed.name) |> toBe(expected.name);
    })
  );
  Expect.(
    test("parse resources object", () => {
      let json = {|
        {
          "resources": {
              "tenants": {
                  "rdoproject.org": {
                      "default-connection": "rdoproject.org",
                      "description": "The rdoproject.org tenant",
                      "url": "https://review.rdoproject.org/manage",
                      "name": "rdoproject.org"
                  }
              },
              "connections": {
                  "github.com": {
                      "base-url": "https://github.com",
                      "type": "github",
                      "name": "github.com"
                  }
              },
              "projects": {
                  "toolbox": {
                      "description": "Unprivileged development environment",
                      "tenant": "local",
                      "name": "toolbox",
                      "source-repositories": []
                  }
              }
          }
      }
      |};
      let parsed = Resources.parse(Json.parseOrRaise(json));
      let isToolbox = (project: Project.project): bool =>
        project.name == "toolbox";
      let maybeToolbox =
        parsed.resources.projects
        ->Belt.List.keep(isToolbox)
        ->Belt.List.get(0);
      let my_assert =
        switch (maybeToolbox) {
        | None => false
        | _ => true
        };
      expect(my_assert) |> toBe(true);
    })
  );
});
