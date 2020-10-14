open Jest;

let read = fn => Node.Fs.readFileSync("tests/fixtures/" ++ fn, `utf8);

let readJson = fn => fn->read->Json.parseOrRaise;

describe("Test resources JSON parsing", () => {
  open Expect;

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
    let expected =
      Tenant.{
        name: "rdoproject.org",
        default_connection: Some("rdoproject.org"),
        description: Some("The rdoproject.org tenant"),
        url: "https://review.rdoproject.org/manage",
      };

    let tenant = Tenant.parse(Json.parseOrRaise(json));
    expect(tenant.name) |> toBe(expected.name);
  });

  test("parse connection object", () => {
    let json = {|
        {
          "base-url": "https://pagure.io",
          "type": "pagure",
          "name": "pagure.io"
        }
      |};
    let expected =
      Connection.{
        base_url: Some("https://pagure.io"),
        connection_type: PAGURE,
        name: "pagure.io",
      };
    let parsed = Connection.parse(Json.parseOrRaise(json));
    expect(parsed.name) |> toBe(expected.name);
  });

  let projectJson = {|
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

  test("parse project object", () => {
    let expected =
      Project.{
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
          {
            name: "containers/toolbox",
            connection: Some("github.com"),
            description: None,
          },
          {
            name: "software-factory/cauth",
            connection: None,
            description: None,
          },
          {
            name: "software-factory/managesf",
            connection: None,
            description: None,
          },
        ],
      };
    let parsed = Project.parse(Json.parseOrRaise(projectJson));
    expect(parsed.name) |> toBe(expected.name);
  });

  test("parse repo object", () => {
    let parsed = Repo.parse(readJson("repo.json"));
    expect(Belt.List.headExn(parsed).name) |> toBe("test/project-distgit");
  });

  test("filter project by tenant", () => {
    let parsed = Project.parse(Json.parseOrRaise(projectJson));
    expect((
      [parsed] |> Project.filterProjectsByTenant("local") |> Belt.List.length,
      [parsed]
      |> Project.filterProjectsByTenant("alternate")
      |> Belt.List.length,
    ))
    |> toEqual((1, 0));
  });

  let parsed = Resources.parse(readJson("resources.json"));
  test("parse resources object", () => {
    let isToolbox = (project: Project.project): bool =>
      project.name == "toolbox";
    let maybeToolbox =
      parsed.resources.projects->Belt.List.keep(isToolbox)->Belt.List.get(0);
    let my_assert =
      switch (maybeToolbox) {
      | None => false
      | _ => true
      };
    expect(my_assert) |> toBe(true);
  });
  test("parse resources repos", () =>
    expect(Belt.List.length(parsed.resources.repos)) |> toBe(1)
  );
});
